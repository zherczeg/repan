/*
 *    Regex Pattern Analyzer
 *
 *    Copyright Zoltan Herczeg (hzmester@freemail.hu). All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this list of
 *      conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this list
 *      of conditions and the following disclaimer in the documentation and/or other materials
 *      provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDER(S) OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "internal.h"
#include "to_string_pcre.h"

typedef struct {
    repan_bracket_node *bracket_node;
    repan_alt_node *alt_node;
    uint32_t modifiers;
} repan_to_string_saved_bracket;

#define UPDATE_MODIFIER_SHIFT 8

#define UPDATE_MODIFIER(modifier, set, chr) \
    current.modifiers = to_string_modifier(context, current.modifiers, \
        REPAN_PARSE_ ## modifier, ((uint32_t)set << UPDATE_MODIFIER_SHIFT) | (uint32_t)(REPAN_CHAR_ ## chr))

static void to_string_decimal(repan_to_string_context *context, uint32_t num)
{
    /* Largest uint32 number is 4294967296 */
    uint8_t buf[10];
    uint8_t *dst = buf + 10;

    do {
        dst--;
        *dst = (num % 10) + REPAN_CHAR_0;
        num /= 10;
    } while (num > 0);

    while (dst < buf + 10) {
        context->write(context, *dst++);
    }
}

static void to_string_hex(repan_to_string_context *context, uint32_t num)
{
    uint8_t buffer[4 + 8];
    uint8_t *dst, *end;

    end = buffer + sizeof(buffer);
    dst = end;

    *(--dst) = REPAN_CHAR_RIGHT_BRACE;

    do {
        uint8_t digit = num & 0xf;

        *(--dst) = (digit >= 10) ? (digit + (REPAN_CHAR_a - 10)) : (digit + REPAN_CHAR_0);
        num >>= 4;
    } while (num > 0);

    *(--dst) = REPAN_CHAR_LEFT_BRACE;
    *(--dst) = REPAN_CHAR_x;
    *(--dst) = REPAN_CHAR_BACKSLASH;

    while (dst < end) {
        context->write(context, *dst++);
    }
}

static void to_string_cstring(repan_to_string_context *context, const char *string)
{
    do {
        context->write(context, *string++);
    } while (*string != REPAN_CHAR_NUL);
}

static void to_string_string(repan_to_string_context *context, repan_string *string)
{
    size_t length = (string->length_and_flags >> REPAN_STRING_LENGTH_SHIFT);
    uint32_t *current = string->string;

    REPAN_ASSERT(length > 0);

    do {
        context->write(context, *current++);
    } while (--length > 0);
}

static void to_string_keyword(repan_to_string_context *context, uint32_t type)
{
    context->write(context, REPAN_CHAR_LEFT_BRACKET);
    context->write(context, REPAN_CHAR_ASTERISK);

    to_string_cstring(context, REPAN_PRIV(keywords)[type].string);
}

static void to_string_u_property(repan_to_string_context *context, uint32_t type)
{
    uint32_t chr;

    context->write(context, REPAN_CHAR_BACKSLASH);

    chr = REPAN_CHAR_p;
    if (type >= REPAN_NEG_U_PROPERTY_CLASS) {
        type -= REPAN_NEG_U_PROPERTY_CLASS;
        chr = REPAN_CHAR_P;
    }

    context->write(context, chr);
    context->write(context, REPAN_CHAR_LEFT_BRACE);

    to_string_cstring(context, REPAN_PRIV(u_properties)[type].string);

    context->write(context, REPAN_CHAR_RIGHT_BRACE);
}

static void to_string_posix_class(repan_to_string_context *context, uint32_t type)
{
    context->write(context, REPAN_CHAR_LEFT_SQUARE_BRACKET);
    context->write(context, REPAN_CHAR_COLON);

    if (type >= REPAN_NEG_POSIX_CLASS) {
        context->write(context, REPAN_CHAR_CIRCUMFLEX_ACCENT);
        type -= REPAN_NEG_POSIX_CLASS;
    }

    to_string_cstring(context, REPAN_PRIV(posix_classes)[type].string);

    context->write(context, REPAN_CHAR_COLON);
    context->write(context, REPAN_CHAR_RIGHT_SQUARE_BRACKET);
}

static uint32_t to_string_modifier(repan_to_string_context *context, uint32_t modifiers,
    uint32_t new_modifier, uint32_t modifier_chr)
{
    REPAN_ASSERT((new_modifier & (new_modifier - 1)) == 0);

    if (modifier_chr & (0xffu << UPDATE_MODIFIER_SHIFT)) {
        if ((modifiers & new_modifier) != 0) {
            return modifiers;
        }
        modifiers |= new_modifier;
    }
    else {
        if ((modifiers & new_modifier) == 0) {
            return modifiers;
        }
        modifiers &= ~new_modifier;
    }

    context->write(context, REPAN_CHAR_LEFT_BRACKET);
    context->write(context, REPAN_CHAR_QUESTION_MARK);

    if (!(modifier_chr & (0xffu << UPDATE_MODIFIER_SHIFT))) {
        context->write(context, REPAN_CHAR_MINUS);
    }

    context->write(context, modifier_chr & ((1u << UPDATE_MODIFIER_SHIFT) - 1));
    context->write(context, REPAN_CHAR_RIGHT_BRACKET);
    return modifiers;
}

static void to_string_class_char(repan_to_string_context *context, uint32_t current_char)
{
    uint32_t escape_char = REPAN_CHAR_NUL;

    switch (current_char) {
    case REPAN_CHAR_BACKSLASH:
    case REPAN_CHAR_LEFT_SQUARE_BRACKET:
    case REPAN_CHAR_RIGHT_SQUARE_BRACKET:
    case REPAN_CHAR_MINUS:
    case REPAN_CHAR_CIRCUMFLEX_ACCENT:
        escape_char = current_char;
        break;
    case REPAN_ESC_a:
        escape_char = REPAN_CHAR_a;
        break;
    case REPAN_ESC_b:
        escape_char = REPAN_CHAR_b;
        break;
    case REPAN_ESC_e:
        escape_char = REPAN_CHAR_e;
        break;
    case REPAN_ESC_f:
        escape_char = REPAN_CHAR_f;
        break;
    case REPAN_ESC_n:
        escape_char = REPAN_CHAR_n;
        break;
    case REPAN_ESC_r:
        escape_char = REPAN_CHAR_r;
        break;
    case REPAN_ESC_t:
        escape_char = REPAN_CHAR_t;
        break;
    }

    if (escape_char != REPAN_CHAR_NUL) {
        context->write(context, REPAN_CHAR_BACKSLASH);
        context->write(context, escape_char);
        return;
    }

    if ((current_char < 0x20)
             || (current_char >= 0x7f && (context->options & REPAN_TO_STRING_ASCII))) {
        to_string_hex(context, current_char);
    }
    else {
        context->write(context, current_char);
    }
}

static void to_string_character_class(repan_to_string_context *context, repan_node *node, uint8_t sub_type)
{
    context->write(context, REPAN_CHAR_LEFT_SQUARE_BRACKET);
    if (sub_type == REPAN_NEGATED_CLASS) {
        context->write(context, REPAN_CHAR_CIRCUMFLEX_ACCENT);
    }

    while (node != NULL) {
        uint32_t escape_char = REPAN_CHAR_NUL;

        switch (node->type) {
        case REPAN_CHAR_NODE:
            to_string_class_char(context, ((repan_char_node*)node)->chr);
            break;

        case REPAN_CHAR_RANGE_NODE:
            to_string_class_char(context, ((repan_char_range_node*)node)->chrs[0]);
            context->write(context, REPAN_CHAR_MINUS);
            to_string_class_char(context, ((repan_char_range_node*)node)->chrs[1]);
            break;

        case REPAN_PERL_CLASS_NODE:
            escape_char = REPAN_PRIV(perl_class_list)[node->sub_type];
            break;

        case REPAN_U_PROPERTY_NODE:
            to_string_u_property(context, node->sub_type);
            break;

        case REPAN_POSIX_CLASS_NODE:
            to_string_posix_class(context, node->sub_type);
            break;

        default:
            REPAN_ASSERT(0);
            break;
        }

        if (escape_char != REPAN_CHAR_NUL) {
            context->write(context, REPAN_CHAR_BACKSLASH);
            context->write(context, escape_char);
        }

        node = node->next_node;
    }

    context->write(context, REPAN_CHAR_RIGHT_SQUARE_BRACKET);
}

static void to_string_bracket_type(repan_to_string_context *context,
    repan_to_string_saved_bracket *current)
{
    repan_bracket_node *bracket_node = current->bracket_node;
    uint32_t type_char;
    uint32_t number;
    const char *string;

    REPAN_ASSERT(REPAN_NODE_ITERABLE_MAX == REPAN_BRACKET_NODE);

    if (bracket_node->sub_type == REPAN_NON_CAPTURING_BRACKET
            && bracket_node->alt_node_list.next_alt_node == NULL
            && bracket_node->alt_node_list.next_node != NULL
            && bracket_node->alt_node_list.next_node->next_node == NULL
            && bracket_node->alt_node_list.next_node->type < REPAN_NODE_ITERABLE_MAX) {
        /* Remove non-capturing brackets who contain a single iterable element. */
        current->modifiers |= REPAN_MODIFIER_HIDE_BRACKET;
        return;
    }

    context->write(context, REPAN_CHAR_LEFT_BRACKET);

    switch (bracket_node->sub_type) {
    case REPAN_CAPTURING_BRACKET:
        return;

    case REPAN_SCRIPT_RUN_BRACKET:
        to_string_keyword(context, REPAN_KEYW_SCRIPT_RUN);
        context->write(context, REPAN_CHAR_COLON);
        return;
    }

    context->write(context, REPAN_CHAR_QUESTION_MARK);

    switch (bracket_node->sub_type) {
    case REPAN_NON_CAPTURING_BRACKET:
        type_char = REPAN_CHAR_COLON;
        break;
    case REPAN_NAMED_CAPTURING_BRACKET:
        context->write(context, REPAN_CHAR_LESS_THAN_SIGN);
        to_string_string(context, ((repan_ext_bracket_node*)bracket_node)->u.name);
        type_char = REPAN_CHAR_GREATER_THAN_SIGN;
        break;
    case REPAN_ATOMIC_BRACKET:
        type_char = REPAN_CHAR_GREATER_THAN_SIGN;
        break;
    case REPAN_CAPTURE_RESET_BRACKET:
        type_char = REPAN_CHAR_PIPE;
        break;
    case REPAN_POSITIVE_LOOKBEHIND_BRACKET:
        context->write(context, REPAN_CHAR_LESS_THAN_SIGN);
        /* Fallthrough. */
    case REPAN_POSITIVE_LOOKAHEAD_BRACKET:
        type_char = REPAN_CHAR_EQUALS_SIGN;
        break;
    case REPAN_NEGATIVE_LOOKBEHIND_BRACKET:
        context->write(context, REPAN_CHAR_LESS_THAN_SIGN);
        /* Fallthrough. */
    case REPAN_NEGATIVE_LOOKAHEAD_BRACKET:
        type_char = REPAN_CHAR_EXCLAMATION_MARK;
        break;
    case REPAN_COND_REFERENCE_BRACKET:
    case REPAN_COND_RECURSE_BRACKET:
        context->write(context, REPAN_CHAR_LEFT_BRACKET);
        if (bracket_node->sub_type == REPAN_COND_RECURSE_BRACKET) {
            context->write(context, REPAN_CHAR_R);
        }

        number = ((repan_ext_bracket_node*)bracket_node)->u.number;
        if (number > 0) {
            to_string_decimal(context, number);
        }
        type_char = REPAN_CHAR_RIGHT_BRACKET;
        break;
    case REPAN_COND_NAMED_REFERENCE_BRACKET:
        context->write(context, REPAN_CHAR_LEFT_BRACKET);
        context->write(context, REPAN_CHAR_LESS_THAN_SIGN);
        to_string_string(context, ((repan_ext_bracket_node*)bracket_node)->u.name);
        context->write(context, REPAN_CHAR_GREATER_THAN_SIGN);
        type_char = REPAN_CHAR_RIGHT_BRACKET;
        break;
    case REPAN_COND_NAMED_RECURSE_BRACKET:
        context->write(context, REPAN_CHAR_LEFT_BRACKET);
        context->write(context, REPAN_CHAR_R);
        context->write(context, REPAN_CHAR_AMPERSAND);
        to_string_string(context, ((repan_ext_bracket_node*)bracket_node)->u.name);
        type_char = REPAN_CHAR_RIGHT_BRACKET;
        break;
    case REPAN_COND_ASSERT_BRACKET:
        return;
    case REPAN_COND_DEFINE_BRACKET:
        string = "(DEFINE)";

        do {
            context->write(context, *string++);
        } while (*string != REPAN_CHAR_NUL);

        return;
    default:
        REPAN_ASSERT(0);
        return;
    }

    context->write(context, type_char);
}

static void to_string_bracket(repan_to_string_context *context)
{
    repan_to_string_saved_bracket current;
    repan_node *node;
    uint32_t it_min;
    uint32_t it_max;

    current.bracket_node = context->pattern->bracket_node;
    current.alt_node = &current.bracket_node->alt_node_list;
    current.modifiers = context->pattern->options & REPAN_CORE_OPTIONS;

    node = current.alt_node->next_node;

    while (REPAN_TRUE) {
        if (node != NULL) {
            uint32_t current_char;
            uint32_t escape_char = REPAN_CHAR_NUL;
            repan_char_class_node *char_class_node;
            repan_node *next_node;

            switch (node->type) {
            case REPAN_CHAR_NODE:
                current_char = ((repan_char_node*)node)->chr;

                switch (current_char) {
                case REPAN_CHAR_BACKSLASH:
                case REPAN_CHAR_LEFT_BRACKET:
                case REPAN_CHAR_RIGHT_BRACKET:
                case REPAN_CHAR_PIPE:
                case REPAN_CHAR_LEFT_SQUARE_BRACKET:
                case REPAN_CHAR_ASTERISK:
                case REPAN_CHAR_PLUS:
                case REPAN_CHAR_QUESTION_MARK:
                case REPAN_CHAR_LEFT_BRACE:
                case REPAN_CHAR_CIRCUMFLEX_ACCENT:
                case REPAN_CHAR_DOLLAR:
                case REPAN_CHAR_DOT:
                    escape_char = current_char;
                    break;
                case REPAN_ESC_a:
                    escape_char = REPAN_CHAR_a;
                    break;
                case REPAN_ESC_e:
                    escape_char = REPAN_CHAR_e;
                    break;
                case REPAN_ESC_f:
                    escape_char = REPAN_CHAR_f;
                    break;
                case REPAN_ESC_n:
                    escape_char = REPAN_CHAR_n;
                    break;
                case REPAN_ESC_r:
                    escape_char = REPAN_CHAR_r;
                    break;
                case REPAN_ESC_t:
                    escape_char = REPAN_CHAR_t;
                    break;
                }

                if (escape_char == REPAN_CHAR_NUL) {
                    UPDATE_MODIFIER(CASELESS, ((repan_char_node*)node)->caseless, i);

                    if ((current_char < 0x20)
                            || (current_char >= 0x7f && (context->options & REPAN_TO_STRING_ASCII))) {
                        to_string_hex(context, current_char);
                    }
                    else {
                        context->write(context, current_char);
                    }
                }
                break;

            case REPAN_PERL_CLASS_NODE:
                escape_char = REPAN_PRIV(perl_class_list)[node->sub_type];
                break;

            case REPAN_U_PROPERTY_NODE:
                to_string_u_property(context, node->sub_type);
                break;

            case REPAN_CHAR_CLASS_NODE:
                char_class_node = (repan_char_class_node*)node;
                UPDATE_MODIFIER(CASELESS, char_class_node->caseless, i);
                next_node = char_class_node->node_list.next_node;
                if (next_node != NULL) {
                    to_string_character_class(context, next_node, node->sub_type);
                }
                else if (node->sub_type == REPAN_NORMAL_CLASS) {
                    to_string_cstring(context, "(*FAIL)");
                }
                else {
                    UPDATE_MODIFIER(DOT_ANY, 1, s);
                    context->write(context, REPAN_CHAR_DOT);
                }
                break;

            case REPAN_DOT_NODE:
                UPDATE_MODIFIER(DOT_ANY, node->sub_type, s);
                context->write(context, REPAN_CHAR_DOT);
                break;

            case REPAN_NEWLINE_NODE:
                escape_char = REPAN_CHAR_R;
                break;

            case REPAN_GRAPHEME_CLUSTER_NODE:
                escape_char = REPAN_CHAR_X;
                break;

            case REPAN_CODE_UNIT_NODE:
                escape_char = REPAN_CHAR_C;
                break;

            case REPAN_BACK_REFERENCE_NODE:
            case REPAN_NAMED_BACK_REFERENCE_NODE:
                context->write(context, REPAN_CHAR_BACKSLASH);
                context->write(context, REPAN_CHAR_g);
                context->write(context, REPAN_CHAR_LEFT_BRACE);

                if (node->type == REPAN_BACK_REFERENCE_NODE) {
                    to_string_decimal(context, ((repan_reference_node*)node)->u.number);
                }
                else {
                    to_string_string(context, ((repan_reference_node*)node)->u.name);
                }

                context->write(context, REPAN_CHAR_RIGHT_BRACE);
                break;

            case REPAN_RECURSE_NODE:
            case REPAN_NAMED_RECURSE_NODE:
                context->write(context, REPAN_CHAR_LEFT_BRACKET);
                context->write(context, REPAN_CHAR_QUESTION_MARK);

                if (node->type == REPAN_RECURSE_NODE) {
                    uint32_t recurse_num = ((repan_reference_node*)node)->u.number;

                    if (recurse_num == 0) {
                        context->write(context, REPAN_CHAR_R);
                    }
                    else {
                        to_string_decimal(context, recurse_num);
                    }
                }
                else {
                    context->write(context, REPAN_CHAR_AMPERSAND);
                    to_string_string(context, ((repan_reference_node*)node)->u.name);
                }

                context->write(context, REPAN_CHAR_RIGHT_BRACKET);
                break;

            case REPAN_BRACKET_NODE:
                if (!REPAN_PRIV(stack_push)(&context->stack)) {
                    context->error = REPAN_ERR_NO_MEMORY;
                    return;
                }

                *REPAN_STACK_TOP_PTR(repan_to_string_saved_bracket, &context->stack) = current;

                current.bracket_node = (repan_bracket_node*)node;
                current.alt_node = &current.bracket_node->alt_node_list;
                node = current.alt_node->next_node;

                to_string_bracket_type(context, &current);
                continue;

            case REPAN_ASSERT_CIRCUMFLEX_NODE:
                UPDATE_MODIFIER(MULTILINE, node->sub_type, m);
                context->write(context, REPAN_CHAR_CIRCUMFLEX_ACCENT);
                break;

            case REPAN_ASSERT_DOLLAR_NODE:
                UPDATE_MODIFIER(MULTILINE, node->sub_type, m);
                context->write(context, REPAN_CHAR_DOLLAR);
                break;

            case REPAN_ASSERT_WORD_BOUNDARY_NODE:
                escape_char = REPAN_CHAR_b;
                break;

            case REPAN_ASSERT_NOT_WORD_BOUNDARY_NODE:
                escape_char = REPAN_CHAR_B;
                break;

            case REPAN_ASSERT_SUBJECT_START_NODE:
                escape_char = REPAN_CHAR_A;
                break;

            case REPAN_ASSERT_MATCH_START_NODE:
                escape_char = REPAN_CHAR_G;
                break;

            case REPAN_ASSERT_SUBJECT_END_NODE:
                escape_char = REPAN_CHAR_z;
                break;

            case REPAN_ASSERT_SUBJECT_END_NEWLINE_NODE:
                escape_char = REPAN_CHAR_Z;
                break;

            case REPAN_VERB_NODE:
            case REPAN_VERB_WITH_ARGUMENT_NODE:
                to_string_keyword(context, node->sub_type);

                if (node->type == REPAN_VERB_WITH_ARGUMENT_NODE) {
                    repan_ext_verb_node *ext_verb_node = (repan_ext_verb_node*)node;
                    uint8_t verb_char = ext_verb_node->arg_separator;

                    context->write(context, verb_char);

                    if (verb_char == REPAN_CHAR_COLON) {
                        to_string_string(context, ext_verb_node->u.name);
                    }
                    else {
                        to_string_decimal(context, ext_verb_node->u.number);
                    }
                }

                context->write(context, REPAN_CHAR_RIGHT_BRACKET);
                break;

            case REPAN_SET_MATCH_START_NODE:
                escape_char = REPAN_CHAR_K;
                break;

            default:
                REPAN_ASSERT(0);
                break;
            }

            if (escape_char != REPAN_CHAR_NUL) {
                context->write(context, REPAN_CHAR_BACKSLASH);
                context->write(context, escape_char);
            }

            node = node->next_node;
            continue;
        }

        if (current.alt_node->next_alt_node != NULL) {
            context->write(context, REPAN_CHAR_PIPE);
            current.alt_node = current.alt_node->next_alt_node;
            node = current.alt_node->next_node;
            continue;
        }

        if (REPAN_STACK_IS_EMPTY(&context->stack)) {
            return;
        }

        if (!(current.modifiers & REPAN_MODIFIER_HIDE_BRACKET)) {
            context->write(context, REPAN_CHAR_RIGHT_BRACKET);
        }
        else {
            uint32_t modifiers = current.modifiers - REPAN_MODIFIER_HIDE_BRACKET;
            REPAN_STACK_TOP(repan_to_string_saved_bracket, &context->stack).modifiers = modifiers;
        }

        it_min = current.bracket_node->it_min;
        it_max = current.bracket_node->it_max;

        if (it_min != 1 || it_max != 1) {
            REPAN_ASSERT(it_min <= it_max);

            if (it_min == 0 && it_max == REPAN_DECIMAL_INF) {
                context->write(context, REPAN_CHAR_ASTERISK);
            }
            else if (it_min == 1 && it_max == REPAN_DECIMAL_INF) {
                context->write(context, REPAN_CHAR_PLUS);
            }
            else if (it_min == 0 && it_max == 1) {
                context->write(context, REPAN_CHAR_QUESTION_MARK);
            }
            else if (it_min != 1 || it_max != 1) {
                context->write(context, REPAN_CHAR_LEFT_BRACE);
                to_string_decimal(context, it_min);

                if (it_min != it_max) {
                    context->write(context, REPAN_CHAR_COMMA);

                    if (it_max != REPAN_DECIMAL_INF) {
                        to_string_decimal(context, it_max);
                    }
                }

                context->write(context, REPAN_CHAR_RIGHT_BRACE);
            }

            if (current.bracket_node->it_type == REPAN_BRACKET_IT_NON_GREEDY) {
                REPAN_ASSERT(it_min != it_max);
                context->write(context, REPAN_CHAR_QUESTION_MARK);
            }
            else if (current.bracket_node->it_type == REPAN_BRACKET_IT_POSSESSIVE) {
                REPAN_ASSERT(it_min != it_max);
                context->write(context, REPAN_CHAR_PLUS);
            }
        }

        node = current.bracket_node->next_node;
        current = REPAN_STACK_TOP(repan_to_string_saved_bracket, &context->stack);
        REPAN_PRIV(stack_pop)(&context->stack);
    }
}

void REPAN_PRIV(repan_to_string_pcre)(repan_to_string_context *context, char_func count_char, char_func write_char)
{
    void *result;

    if (context->pattern->options & REPAN_PATTERN_DAMAGED) {
        context->error = REPAN_ERR_DAMAGED_PATTERN;
        return;
    }

    context->error = REPAN_SUCCESS;

    REPAN_PRIV(stack_init)(&context->stack, sizeof(repan_to_string_saved_bracket));

    context->char_count = 1;
    context->write = count_char;
    to_string_bracket(context);

    if (context->error != REPAN_SUCCESS) {
        return;
    }

    result = malloc(context->char_count * context->char_size);

    if (result == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        return;
    }

    context->result = result;

    context->write = write_char;
    to_string_bracket(context);
    write_char(context, REPAN_CHAR_NUL);

    REPAN_ASSERT(context->error == REPAN_SUCCESS);
    REPAN_ASSERT(((uint8_t*)result) + (context->char_count * context->char_size) == (uint8_t*)context->result);

    context->result = result;
    REPAN_PRIV(stack_free)(&context->stack);
}
