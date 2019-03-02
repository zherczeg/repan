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
#include "parser.h"

typedef struct {
    repan_bracket_node *bracket_node;
    repan_alt_node *last_alt_node;
    uint32_t *bracket_start;
    uint32_t modifiers;
    union {
        uint32_t capture_reset;
        uint32_t alt_limit;
    } u1;
    union {
        uint32_t capture_max;
        uint32_t is_cond_assert;
    } u2;
} repan_parser_saved_bracket;

enum {
    REPAN_BRACKET_CMD_NONE,
    REPAN_BRACKET_CMD_READ_PATTERN_CONFIG,
    REPAN_BRACKET_CMD_READ_GROUP_NAME,
    REPAN_BRACKET_CMD_COND_GENERIC,
    REPAN_BRACKET_CMD_COND_ASSERT,
    REPAN_BRACKET_CMD_COND_DEFINE,
    REPAN_BRACKET_CMD_SET_MODIFIERS,
    REPAN_BRACKET_CMD_CAPTURE_RESET,
};

typedef struct {
    repan_parser_saved_bracket current;
    repan_node *last_node;
    uint32_t capture_count;
    uint32_t highest_capture_ref;
    uint32_t *highest_capture_ref_start;

    /* Values of prev_node:
       NULL - iterating is disallowed
       previous node - last_node is iterable
       last_node - last_node is bracket */
    repan_prev_node *prev_node;

    /* These members are for constructing new brackets. */
    uint32_t *bracket_start;
    uint16_t bracket_size;
    uint8_t bracket_type;
    uint8_t bracket_command;
    uint32_t bracket_modifiers;
} repan_parser_locals;

static repan_string *parse_add_name_reference(repan_parser_context *context, uint32_t *start, size_t length)
{
    repan_string *name = REPAN_PRIV(string_add)(&context->result->bracket_names, start, length, &context->error);

    if (name == NULL) {
        return NULL;
    }

    if ((name->length_and_flags & (REPAN_STRING_DEFINED | REPAN_STRING_REFERENCED)) == 0) {
        /* A new, not yet defined string. */
        repan_undefined_name *undefined_name = REPAN_ALLOC(repan_undefined_name, context->result);

        if (undefined_name == NULL) {
            context->error = REPAN_ERR_NO_MEMORY;
            return NULL;
        }

        undefined_name->next = context->undefined_names;
        undefined_name->start = context->pattern;
        undefined_name->name = name;

        context->undefined_names = undefined_name;
    }

    name->length_and_flags |= REPAN_STRING_REFERENCED;
    return name;
}

#include "parser_pcre_char_inl.c"

enum {
    REPAN_MODIFIER_MODE_SET,
    REPAN_MODIFIER_MODE_CLEAR,
    REPAN_MODIFIER_MODE_RESET,
};

static void parse_open_bracket(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t current_char = *context->pattern;
    uint32_t *pattern, *pattern_end;
    uint32_t new_modifiers;
    int is_reference, modifier_mode;
    const char *string;

    if (current_char == REPAN_CHAR_ASTERISK) {
        uint32_t *keyword = context->pattern + 1;
        repan_string *name = NULL;
        repan_node *verb_node;
        size_t size;
        uint32_t data;

        pattern = keyword;
        while (*pattern == '_' || REPAN_IS_CASELESS_LATIN(*pattern)) {
            pattern++;
        }

        if (pattern == keyword) {
            data = REPAN_KEYW_MARK | REPAN_KEYW_OPT_HAS_ARGUMENT;
        }
        else {
            data = REPAN_PRIV(find_keyword)(keyword, pattern - keyword);
        }

        if (data == 0 || (data & REPAN_KEYW_OPT_INIT)) {
            context->error = REPAN_ERR_UNKNOWN_VERB;
            context->pattern--;
            return;
        }

        if (data & REPAN_KEYW_OPT_BRACKET) {
            REPAN_ASSERT((data & REPAN_KEYW_TYPE_MASK) == REPAN_KEYW_SCRIPT_RUN);

            if (*pattern != REPAN_CHAR_COLON) {
                context->error = REPAN_ERR_COLON_EXPECTED;
                context->pattern = pattern;
                return;
            }

            locals->bracket_size = (uint16_t)sizeof(repan_bracket_node);
            locals->bracket_type = REPAN_SCRIPT_RUN_BRACKET;
            locals->bracket_command = REPAN_BRACKET_CMD_NONE;
            context->pattern = pattern + 1;
            return;
        }

        if ((data & REPAN_KEYW_OPT_HAS_ARGUMENT) && *pattern == REPAN_CHAR_COLON) {
            uint32_t *pattern_end = context->pattern_end;

            pattern++;
            context->pattern = pattern;

            while (*pattern != REPAN_CHAR_RIGHT_BRACKET) {
                pattern++;

                if (pattern >= pattern_end) {
                    context->error = REPAN_ERR_RIGHT_BRACKET_EXPECTED;
                    context->pattern = pattern;
                    return;
                }
            }

            if (context->pattern < pattern) {
                name = REPAN_PRIV(string_add)(&context->result->verb_arguments,
                    context->pattern, (size_t)(pattern - context->pattern), &context->error);

                if (name == NULL) {
                    return;
                }
            }
            else if ((data & REPAN_KEYW_TYPE_MASK) == REPAN_KEYW_MARK) {
                context->error = REPAN_ERR_MARK_ARGUMENT_EXPECTED;
                return;
            }
        }
        else {
            if ((data & REPAN_KEYW_TYPE_MASK) == REPAN_KEYW_MARK) {
                context->error = REPAN_ERR_COLON_EXPECTED;
                context->pattern = pattern;
                return;
            }

            if (*pattern != REPAN_CHAR_RIGHT_BRACKET) {
                context->error = REPAN_ERR_RIGHT_BRACKET_EXPECTED;
                context->pattern = pattern;
                return;
            }
        }

        size = (name == NULL) ? sizeof(repan_node) : sizeof(repan_ext_verb_node);
        verb_node = (repan_node*)REPAN_PRIV(alloc)(context->result, size);

        if (verb_node == NULL) {
            context->error = REPAN_ERR_NO_MEMORY;
            context->pattern--;
            return;
        }

        verb_node->next_node = NULL;
        verb_node->type = REPAN_VERB_NODE;
        verb_node->sub_type = (uint8_t)(data & REPAN_KEYW_TYPE_MASK);

        if (name != NULL) {
            verb_node->type = REPAN_VERB_WITH_ARGUMENT_NODE;
            ((repan_ext_verb_node*)verb_node)->arg_separator = REPAN_CHAR_COLON;
            ((repan_ext_verb_node*)verb_node)->u.name = name;
        }

        locals->last_node->next_node = verb_node;
        locals->last_node = verb_node;

        context->pattern = pattern + 1;
        return;
    }

    locals->bracket_size = (uint16_t)sizeof(repan_bracket_node);
    locals->bracket_type = REPAN_NON_CAPTURING_BRACKET;
    locals->bracket_command = REPAN_BRACKET_CMD_NONE;

    if (current_char != REPAN_CHAR_QUESTION_MARK) {
        if (!(locals->current.modifiers & REPAN_MODIFIER_NO_AUTOCAPTURE)) {
            locals->bracket_type = REPAN_CAPTURING_BRACKET;

            if (++locals->capture_count > REPAN_RESOURCE_MAX) {
                context->error = REPAN_ERR_CAPTURE_LIMIT;
                context->pattern--;
                return;
            }
        }
        return;
    }

    pattern = context->pattern + 2;
    is_reference = REPAN_IS_DECIMAL_DIGIT(pattern[-1]);

    switch (pattern[-1]) {
    case REPAN_CHAR_COLON:
        context->pattern = pattern;
        return;
    case REPAN_CHAR_GREATER_THAN_SIGN:
        locals->bracket_type = REPAN_ATOMIC_BRACKET;
        context->pattern = pattern;
        return;
    case REPAN_CHAR_PIPE:
        locals->bracket_type = REPAN_CAPTURE_RESET_BRACKET;
        locals->bracket_command = REPAN_BRACKET_CMD_CAPTURE_RESET;
        context->pattern = pattern;
        return;
    case REPAN_CHAR_EQUALS_SIGN:
        locals->bracket_type = REPAN_POSITIVE_LOOKAHEAD_BRACKET;
        context->pattern = pattern;
        return;
    case REPAN_CHAR_EXCLAMATION_MARK:
        locals->bracket_type = REPAN_NEGATIVE_LOOKAHEAD_BRACKET;
        context->pattern = pattern;
        return;
    case REPAN_CHAR_LESS_THAN_SIGN:
        if (pattern[0] == REPAN_CHAR_EQUALS_SIGN) {
            locals->bracket_type = REPAN_POSITIVE_LOOKBEHIND_BRACKET;
            context->pattern = pattern + 1;
            return;
        }
        else if (pattern[0] == REPAN_CHAR_EXCLAMATION_MARK) {
            locals->bracket_type = REPAN_NEGATIVE_LOOKBEHIND_BRACKET;
            context->pattern = pattern + 1;
            return;
        }
        /* Fallthrough. */
    case REPAN_CHAR_APOSTROPHE:
        locals->bracket_size = (uint16_t)sizeof(repan_ext_bracket_node);
        locals->bracket_type = REPAN_NAMED_CAPTURING_BRACKET;
        locals->bracket_command = REPAN_BRACKET_CMD_READ_GROUP_NAME;

        if (++locals->capture_count > REPAN_RESOURCE_MAX) {
            context->error = REPAN_ERR_CAPTURE_LIMIT;
            context->pattern--;
            return;
        }
        context->pattern = pattern;
        return;
    case REPAN_CHAR_PLUS:
    case REPAN_CHAR_R:
    case REPAN_CHAR_AMPERSAND:
        is_reference = REPAN_TRUE;
        break;
    case REPAN_CHAR_MINUS:
        if (REPAN_IS_DECIMAL_DIGIT(pattern[0])) {
            is_reference = REPAN_TRUE;
        }
        break;
    case REPAN_CHAR_P:
        if (pattern[0] == REPAN_CHAR_GREATER_THAN_SIGN
                || pattern[0] == REPAN_CHAR_EQUALS_SIGN) {
            is_reference = REPAN_TRUE;
        }
        else if (pattern[0] == REPAN_CHAR_LESS_THAN_SIGN) {
            locals->bracket_size = (uint16_t)sizeof(repan_ext_bracket_node);
            locals->bracket_type = REPAN_NAMED_CAPTURING_BRACKET;
            locals->bracket_command = REPAN_BRACKET_CMD_READ_GROUP_NAME;
            context->pattern = pattern + 1;
            return;
        }
        break;
    case REPAN_CHAR_LEFT_BRACKET:
        context->pattern = pattern - 1;

        if (pattern[0] == REPAN_CHAR_QUESTION_MARK) {
            locals->bracket_type = REPAN_COND_ASSERT_BRACKET;
            locals->bracket_command = REPAN_BRACKET_CMD_COND_ASSERT;

            if (pattern[1] == REPAN_CHAR_EQUALS_SIGN || pattern[1] == REPAN_CHAR_EXCLAMATION_MARK) {
                return;
            }

            if (pattern[1] == REPAN_CHAR_LESS_THAN_SIGN
                    && (pattern[2] == REPAN_CHAR_EQUALS_SIGN || pattern[2] == REPAN_CHAR_EXCLAMATION_MARK)) {
                return;
            }

            context->error = REPAN_ERR_INVALID_COND_ASSERT;
            return;
        }

        string = "DEFINE)";

        while (REPAN_TRUE) {
            if (*string == REPAN_CHAR_NUL) {
                locals->bracket_type = REPAN_COND_DEFINE_BRACKET;
                locals->bracket_command = REPAN_BRACKET_CMD_COND_DEFINE;
                return;
            }
            if (*pattern++ != *string++) {
                break;
            }
        }

        locals->bracket_size = (uint16_t)sizeof(repan_ext_bracket_node);
        locals->bracket_command = REPAN_BRACKET_CMD_COND_GENERIC;
        return;
    case REPAN_CHAR_HASHMARK:
        pattern_end = context->pattern_end;

        while (*pattern != REPAN_CHAR_RIGHT_BRACKET) {
            pattern++;

            if (pattern >= pattern_end) {
                context->error = REPAN_ERR_UNTERMINATED_COMMENT;
                context->pattern--;
                return;
            }
        }

        locals->bracket_size = 0;
        context->pattern = pattern + 1;
        return;
    }

    if (is_reference) {
        locals->bracket_size = 0;
        context->pattern++;
        parse_reference(context, locals);
        return;
    }

    new_modifiers = locals->current.modifiers;
    modifier_mode = REPAN_MODIFIER_MODE_SET;

    if (pattern[-1] == REPAN_CHAR_CIRCUMFLEX_ACCENT) {
        modifier_mode = REPAN_MODIFIER_MODE_RESET;
        new_modifiers = 0;
        pattern++;
    }

    while (REPAN_TRUE) {
        uint32_t modifier_flag;
        const uint8_t *modifier_char;

        current_char = pattern[-1];

        switch (current_char) {
        case REPAN_CHAR_RIGHT_BRACKET:
            locals->bracket_size = 0;
            context->pattern = pattern;

            locals->current.modifiers = new_modifiers;
            return;
        case REPAN_CHAR_COLON:
            if (locals->current.modifiers != new_modifiers) {
                locals->bracket_modifiers = new_modifiers;
                locals->bracket_command = REPAN_BRACKET_CMD_SET_MODIFIERS;
            }
            context->pattern = pattern;
            return;
        case REPAN_CHAR_x:
            pattern++;

            if (modifier_mode != REPAN_MODIFIER_MODE_CLEAR) {
                if (pattern[-1] == REPAN_CHAR_x) {
                    new_modifiers |= REPAN_MODIFIER_EXTENDED_MORE;
                    pattern++;
                }
                else {
                    new_modifiers |= REPAN_MODIFIER_EXTENDED;
                }
            }
            else {
                new_modifiers &= ~REPAN_MODIFIER_EXTENDED_MORE;
            }
            continue;
        case REPAN_CHAR_MINUS:
            if (modifier_mode != REPAN_MODIFIER_MODE_SET) {
                context->error = REPAN_ERR_INVALID_HYPEN_IN_OPTION_LIST;
                context->pattern = pattern - 1;
                return;
            }

            modifier_mode = REPAN_MODIFIER_MODE_CLEAR;
            pattern++;
            continue;
        }

        modifier_flag = 0x1;
        modifier_char = REPAN_PRIV(modifier_list);

        while (REPAN_TRUE) {
            if (*modifier_char == REPAN_CHAR_NUL) {
                if (context->pattern_end < pattern) {
                    context->error = REPAN_ERR_UNTERMINATED_OPTION_LIST;
                    context->pattern--;
                    return;
                }

                context->error = REPAN_ERR_UNKNOWN_OPTION;
                context->pattern = pattern - 1;
                return;
            }

            if (*modifier_char == current_char) {
                break;
            }

            modifier_char++;
            modifier_flag <<= 1;
        }

        if (modifier_mode == REPAN_MODIFIER_MODE_CLEAR) {
            new_modifiers &= ~modifier_flag;
        }
        else {
            new_modifiers |= modifier_flag;
        }
        pattern++;
    }
}

enum {
    REPAN_CONDITION_NAME,
    REPAN_CONDITION_NUMBER,
    REPAN_CONDITION_NON_ZERO_NUMBER,
};

static void parse_config(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t *pattern = context->pattern;

    while (pattern[0] == REPAN_CHAR_LEFT_BRACKET && pattern[1] == REPAN_CHAR_ASTERISK) {
        uint32_t *keyword;
        repan_node *verb_node;
        size_t size;
        uint32_t data, number;

        pattern += 2;
        keyword = pattern;

        while (*pattern == '_' || REPAN_IS_CASELESS_LATIN(*pattern)) {
            pattern++;
        }

        if (keyword == pattern) {
            return;
        }

        data = REPAN_PRIV(find_keyword)(keyword, pattern - keyword);

        if (data == 0) {
            context->error = REPAN_ERR_UNKNOWN_VERB;
            context->pattern = keyword - 2;
            return;
        }

        if (!(data & REPAN_KEYW_OPT_INIT)) {
            return;
        }

        number = 0;
        if (data & REPAN_KEYW_OPT_HAS_ARGUMENT) {
            if (*pattern != REPAN_CHAR_EQUALS_SIGN) {
                context->error = REPAN_ERR_EQUALS_SIGN_EXPECTED;
                break;
            }

            pattern++;

            if (!REPAN_IS_DECIMAL_DIGIT(*pattern)) {
                context->error = REPAN_ERR_UNSIGNED_INTEGER_REQUIRED;
                break;
            }

            number = REPAN_PRIV(parse_decimal)(&pattern);
        }

        if (*pattern != REPAN_CHAR_RIGHT_BRACKET) {
            context->error = REPAN_ERR_RIGHT_BRACKET_EXPECTED;
            break;
        }

        pattern++;

        size = (data & REPAN_KEYW_OPT_HAS_ARGUMENT) ? sizeof(repan_ext_verb_node) : sizeof(repan_node);
        verb_node = (repan_node*)REPAN_PRIV(alloc)(context->result, size);

        if (verb_node == NULL) {
            context->error = REPAN_ERR_NO_MEMORY;
            break;
        }

        verb_node->next_node = NULL;
        verb_node->type = REPAN_VERB_NODE;
        verb_node->sub_type = (uint8_t)(data & REPAN_KEYW_TYPE_MASK);

        if (data & REPAN_KEYW_OPT_HAS_ARGUMENT) {
            verb_node->type = REPAN_VERB_WITH_ARGUMENT_NODE;
            ((repan_ext_verb_node*)verb_node)->arg_separator = REPAN_CHAR_EQUALS_SIGN;
            ((repan_ext_verb_node*)verb_node)->u.number = number;
        }

        locals->last_node->next_node = verb_node;
        locals->last_node = verb_node;
    }

    context->pattern = pattern;
}

static void parse_group_name(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t *pattern = context->pattern;
    repan_string *name;
    uint32_t terminator_char;

    ((repan_ext_bracket_node*)locals->current.bracket_node)->u.name = NULL;

    if (pattern[-1] == REPAN_CHAR_LESS_THAN_SIGN) {
        terminator_char = REPAN_CHAR_GREATER_THAN_SIGN;
    }
    else {
        REPAN_ASSERT(pattern[-1] == REPAN_CHAR_APOSTROPHE);

        terminator_char = REPAN_CHAR_APOSTROPHE;
    }

    if (*pattern == terminator_char
            || REPAN_IS_DECIMAL_DIGIT(*pattern)) {
        context->error = REPAN_ERR_NAME_EXPECTED;
        return;
    }

    while (REPAN_PRIV(is_word_char)(context->result, *pattern)) {
        pattern++;
    }

    if (*pattern != terminator_char) {
        context->error = REPAN_ERR_INVALID_NAME_CHAR;
        context->pattern = pattern;
        return;
    }

    name = REPAN_PRIV(string_add)(&context->result->bracket_names,
        context->pattern, (size_t)(pattern - context->pattern), &context->error);

    if (name == NULL) {
        return;
    }

    name->length_and_flags |= REPAN_STRING_DEFINED;

    context->pattern = pattern + 1;
    ((repan_ext_bracket_node*)locals->current.bracket_node)->u.name = name;
}

static void parse_condition(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t terminator_char = REPAN_CHAR_RIGHT_BRACKET;
    uint32_t *pattern = context->pattern;
    repan_string *name;
    uint32_t *name_start;
    int condition = REPAN_CONDITION_NON_ZERO_NUMBER;

    REPAN_ASSERT(pattern[0] == REPAN_CHAR_LEFT_BRACKET);

    pattern++;

    if (pattern[0] == REPAN_CHAR_R) {
        locals->current.bracket_node->sub_type = REPAN_COND_RECURSE_BRACKET;
        condition = REPAN_CONDITION_NUMBER;
        pattern++;

        if (pattern[0] == REPAN_CHAR_AMPERSAND) {
            locals->current.bracket_node->sub_type = REPAN_COND_NAMED_RECURSE_BRACKET;
            condition = REPAN_CONDITION_NAME;
            pattern++;
        }
        else if (pattern[0] == REPAN_CHAR_RIGHT_BRACKET) {
            context->pattern = pattern + 1;
            ((repan_ext_bracket_node*)locals->current.bracket_node)->u.number = 0;
            return;
        }
    }
    else {
        locals->current.bracket_node->sub_type = REPAN_COND_REFERENCE_BRACKET;

        if (pattern[0] == REPAN_CHAR_LESS_THAN_SIGN || pattern[0] == REPAN_CHAR_APOSTROPHE) {
            if (pattern[0] == REPAN_CHAR_LESS_THAN_SIGN) {
                terminator_char = REPAN_CHAR_GREATER_THAN_SIGN;
            }
            else {
                terminator_char = REPAN_CHAR_APOSTROPHE;
            }
            locals->current.bracket_node->sub_type = REPAN_COND_NAMED_REFERENCE_BRACKET;
            condition = REPAN_CONDITION_NAME;
            pattern++;
        }
    }

    if (condition != REPAN_CONDITION_NAME) {
        uint32_t ref_num;
        int sign = 0;

        switch (*pattern) {
        case REPAN_CHAR_PLUS:
            sign = 1;
            pattern++;
            break;
        case REPAN_CHAR_MINUS:
            sign = -1;
            pattern++;
            break;
        }

        if (!REPAN_IS_DECIMAL_DIGIT(*pattern)) {
            context->error = REPAN_ERR_SIGNED_INTEGER_REQUIRED;

            if (sign != 0) {
                pattern--;
            }

            context->pattern = pattern;
            return;
        }

        ref_num = REPAN_PRIV(parse_decimal)(&pattern);

        if (ref_num == 0 && sign != 0) {
            context->error = REPAN_ERR_SIGNED_ZERO_IS_NOT_ALLOWED;
            return;
        }

        if (ref_num == REPAN_DECIMAL_INF || (ref_num == 0 && condition == REPAN_CONDITION_NON_ZERO_NUMBER)) {
            context->error = REPAN_ERR_CAPTURING_BRACKET_NOT_EXIST;
            return;
        }

        REPAN_ASSERT(terminator_char == REPAN_CHAR_RIGHT_BRACKET);

        if (*pattern != REPAN_CHAR_RIGHT_BRACKET) {
            context->error = REPAN_ERR_RIGHT_BRACKET_EXPECTED;
            context->pattern = pattern;
            return;
        }

        if (sign == -1) {
            ref_num--;
            if (ref_num >= locals->capture_count) {
                context->error = REPAN_ERR_CAPTURING_BRACKET_NOT_EXIST;
                return;
            }
            ref_num = locals->capture_count - ref_num;
        }
        else {
            if (sign == 1) {
                ref_num += locals->capture_count;
            }

            if (ref_num > locals->highest_capture_ref) {
                locals->highest_capture_ref = ref_num;
                locals->highest_capture_ref_start = context->pattern;
            }
        }

        ((repan_ext_bracket_node*)locals->current.bracket_node)->u.number = ref_num;
        context->pattern = pattern + 1;
        return;
    }

    name_start = pattern;

    if (REPAN_IS_DECIMAL_DIGIT(*pattern) || !REPAN_PRIV(is_word_char)(context->result, *pattern)) {
        context->error = REPAN_ERR_NAME_EXPECTED;
        context->pattern = pattern;
        return;
    }

    while (REPAN_PRIV(is_word_char)(context->result, *pattern)) {
        pattern++;
    }

    if (*pattern != terminator_char) {
        context->error = REPAN_ERR_INVALID_NAME_CHAR;
        context->pattern = pattern;
        return;
    }

    name = parse_add_name_reference(context, name_start, (size_t)(pattern - name_start));

    if (name == NULL) {
        return;
    }

    if (terminator_char != REPAN_CHAR_RIGHT_BRACKET) {
        pattern++;
        if (*pattern != REPAN_CHAR_RIGHT_BRACKET) {
            context->error = REPAN_ERR_RIGHT_BRACKET_EXPECTED;
            context->pattern = pattern;
            return;
        }
    }

    ((repan_ext_bracket_node*)locals->current.bracket_node)->u.name = name;
    context->pattern = pattern + 1;
}

static void parse_exec_command(repan_parser_context *context, repan_parser_locals *locals)
{
    switch (locals->bracket_command) {
    case REPAN_BRACKET_CMD_READ_PATTERN_CONFIG:
        parse_config(context, locals);
        return;

    case REPAN_BRACKET_CMD_READ_GROUP_NAME:
        parse_group_name(context, locals);
        return;

    case REPAN_BRACKET_CMD_COND_GENERIC:
        parse_condition(context, locals);
        return;

    case REPAN_BRACKET_CMD_COND_ASSERT:
        locals->current.u1.alt_limit = 2;
        locals->current.u2.is_cond_assert = REPAN_TRUE;
        return;

    case REPAN_BRACKET_CMD_COND_DEFINE:
        locals->current.u1.alt_limit = 1;
        context->pattern += 8;
        return;

    case REPAN_BRACKET_CMD_SET_MODIFIERS:
        locals->current.modifiers = locals->bracket_modifiers;
        return;

    case REPAN_BRACKET_CMD_CAPTURE_RESET:
        locals->current.u1.capture_reset = locals->capture_count;
        locals->current.u2.capture_max = locals->capture_count;
        return;

    default:
        REPAN_ASSERT(0);
        return;
    }
}

void REPAN_PRIV(parse_pcre_bracket)(repan_parser_context *context)
{
    repan_parser_locals locals;

    REPAN_PRIV(stack_init)(&context->stack, sizeof(repan_parser_saved_bracket));

    locals.last_node = (repan_node*)&context->result->bracket_node;
    locals.capture_count = 0;
    locals.highest_capture_ref = 0;
    locals.highest_capture_ref_start = NULL;

    locals.bracket_size = (uint16_t)sizeof(repan_bracket_node);
    locals.bracket_type = REPAN_NON_CAPTURING_BRACKET;
    locals.bracket_command = REPAN_BRACKET_CMD_READ_PATTERN_CONFIG;

    locals.current.bracket_start = context->pattern;
    locals.current.modifiers = 0;

    while (REPAN_TRUE) {
        locals.current.bracket_node = (repan_bracket_node*)REPAN_PRIV(alloc)(context->result, locals.bracket_size);

        if (locals.current.bracket_node == NULL) {
            context->error = REPAN_ERR_NO_MEMORY;
            context->pattern = locals.current.bracket_start;
            return;
        }

        locals.last_node->next_node = (repan_node*)locals.current.bracket_node;

        locals.current.bracket_node->next_node = NULL;
        locals.current.bracket_node->type = REPAN_BRACKET_NODE;
        locals.current.bracket_node->sub_type = locals.bracket_type;
        locals.current.bracket_node->it_type = REPAN_BRACKET_IT_GREEDY;
        locals.current.bracket_node->it_min = 1;
        locals.current.bracket_node->it_max = 1;

        locals.current.last_alt_node = &locals.current.bracket_node->alt_node_list;
        locals.current.last_alt_node->next_node = NULL;
        locals.current.last_alt_node->next_alt_node = NULL;

        locals.last_node = (repan_node*)locals.current.last_alt_node;
        locals.prev_node = NULL;

        if (locals.bracket_command != REPAN_BRACKET_CMD_NONE) {
            parse_exec_command(context, &locals);

            if (context->error != REPAN_SUCCESS) {
                if (context->error == REPAN_ERR_NO_MEMORY) {
                    context->pattern = locals.current.bracket_start;
                }
                return;
            }
        }

        while (REPAN_TRUE) {
            uint32_t current_char;
            repan_alt_node *new_alt_node;

            if (context->pattern >= context->pattern_end) {
                if (!REPAN_STACK_IS_EMPTY(&context->stack)) {
                    context->error = REPAN_ERR_UNTERMINATED_BRACKET;
                    context->pattern = locals.current.bracket_start;
                }
                else if (locals.capture_count < locals.highest_capture_ref) {
                    context->error = REPAN_ERR_CAPTURING_BRACKET_NOT_EXIST;
                    context->pattern = locals.highest_capture_ref_start;
                }

                context->result->capture_count = locals.capture_count;
                return;
            }

            current_char = *context->pattern;

            switch (current_char) {
            case REPAN_CHAR_LEFT_BRACKET:
                locals.bracket_start = context->pattern;
                locals.bracket_size = 0;

                context->pattern++;

                parse_open_bracket(context, &locals);

                if (context->error != REPAN_SUCCESS) {
                    return;
                }

                if (locals.bracket_size == 0) {
                    continue;
                }
                goto push_new_bracket;

            case REPAN_CHAR_RIGHT_BRACKET:
                if (REPAN_STACK_IS_EMPTY(&context->stack)) {
                    context->error = REPAN_ERR_UNEXPECTED_BRACKET_CLOSE;
                    return;
                }

                if (locals.current.bracket_node->sub_type == REPAN_CAPTURE_RESET_BRACKET
                        && locals.capture_count < locals.current.u2.capture_max) {
                    locals.capture_count = locals.current.u2.capture_max;
                }

                locals.last_node = (repan_node*)locals.current.bracket_node;
                locals.prev_node = (repan_prev_node*)locals.current.bracket_node;
                locals.current = REPAN_STACK_TOP(repan_parser_saved_bracket, &context->stack);
                REPAN_PRIV(stack_pop)(&context->stack);

                if (locals.current.bracket_node->sub_type == REPAN_COND_ASSERT_BRACKET) {
                    if (locals.current.u2.is_cond_assert) {
                        locals.prev_node = NULL;
                    }
                    locals.current.u2.is_cond_assert = REPAN_FALSE;
                }

                context->pattern++;
                continue;

            case REPAN_CHAR_PIPE:
                if (locals.current.bracket_node->sub_type >= REPAN_COND_BRACKET_START) {
                    locals.current.u1.alt_limit--;

                    if (locals.current.u1.alt_limit == 0) {
                        if (locals.current.bracket_node->sub_type == REPAN_COND_DEFINE_BRACKET) {
                            context->error = REPAN_ERR_DEFINE_HAS_ALTERNATIVES;
                            return;
                        }
                        context->error = REPAN_ERR_TOO_MANY_ALTERNATIVES;
                        return;
                    }
                }

                new_alt_node = REPAN_ALLOC(repan_alt_node, context->result);

                if (new_alt_node == NULL) {
                    context->error = REPAN_ERR_NO_MEMORY;
                    context->pattern = locals.current.bracket_start;
                    return;
                }

                new_alt_node->next_node = NULL;
                new_alt_node->next_alt_node = NULL;

                locals.current.last_alt_node->next_alt_node = new_alt_node;
                locals.current.last_alt_node = new_alt_node;
                locals.last_node = (repan_node*)new_alt_node;

                context->pattern++;

                if (locals.current.bracket_node->sub_type == REPAN_CAPTURE_RESET_BRACKET) {
                    if (locals.current.u2.capture_max < locals.capture_count) {
                        locals.current.u2.capture_max = locals.capture_count;
                    }
                    locals.capture_count = locals.current.u1.capture_reset;
                }
                continue;

            case REPAN_CHAR_LEFT_SQUARE_BRACKET:
                locals.prev_node = (repan_prev_node*)locals.last_node;
                parse_char_range(context, &locals);
                break;

            case REPAN_CHAR_ASTERISK:
            case REPAN_CHAR_PLUS:
            case REPAN_CHAR_QUESTION_MARK:
            case REPAN_CHAR_LEFT_BRACE:
                if (REPAN_PRIV(parse_repeat)(context, &locals.last_node, &locals.prev_node)) {
                    continue;
                }

                if (context->error != REPAN_SUCCESS) {
                    return;
                }

                REPAN_ASSERT(current_char == REPAN_CHAR_LEFT_BRACE);
                /* Fallthrough. */

            default:
                parse_character(context, &locals, current_char);
                break;
            }

            if (context->error != REPAN_SUCCESS) {
                return;
            }
        }

push_new_bracket:
        if (!REPAN_PRIV(stack_push)(&context->stack)) {
            context->error = REPAN_ERR_NO_MEMORY;
            return;
        }

        REPAN_STACK_TOP(repan_parser_saved_bracket, &context->stack) = locals.current;

        locals.current.bracket_start = locals.bracket_start;
    }
}
