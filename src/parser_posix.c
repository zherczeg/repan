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
} repan_parser_saved_bracket;

typedef struct {
    repan_parser_saved_bracket current;
    repan_node *last_node;
    uint32_t capture_starts;
    uint32_t capture_ends;

    /* Values of prev_node:
       previous node - last_node is iterable
       last_node - last_node is bracket */
    repan_prev_node *prev_node;

    /* These members are for constructing new brackets. */
    uint32_t *bracket_start;
    uint8_t bracket_type;
} repan_parser_locals;

static int parse_repeat(repan_parser_context *context, repan_parser_locals *locals)
{
    repan_bracket_node *bracket_node;
    uint32_t *pattern = context->pattern;
    uint32_t it_min, it_max;
    uint32_t current_char = *pattern;

    REPAN_ASSERT(locals->last_node->next_node == NULL);

    if (current_char != REPAN_CHAR_LEFT_BRACE) {
        if (current_char == REPAN_CHAR_ASTERISK) {
            it_min = 0;
            it_max = REPAN_DECIMAL_INF;
        }
        else if (current_char == REPAN_CHAR_PLUS) {
            it_min = 1;
            it_max = REPAN_DECIMAL_INF;
        }
        else {
            REPAN_ASSERT(current_char == REPAN_CHAR_QUESTION_MARK);
            it_min = 0;
            it_max = 1;
        }

        pattern++;
    }
    else {
        pattern++;

        if (REPAN_IS_DECIMAL_DIGIT(*pattern)) {
            it_min = REPAN_PRIV(parse_decimal)(&pattern);
            if (it_min == REPAN_DECIMAL_INF) {
                context->error = REPAN_ERR_TOO_BIG_QUANTIFIER;
                context->pattern = pattern;
                return REPAN_FALSE;
            }
        }
        else if (*pattern == REPAN_CHAR_COMMA) {
            it_min = 0;
        }
        else {
            context->error = REPAN_ERR_POSIX_INVALID_QUANTIFIER;
            return REPAN_FALSE;
        }

        it_max = it_min;
        if (*pattern == REPAN_CHAR_COMMA) {
            pattern++;
            it_max = REPAN_DECIMAL_INF;

            if (REPAN_IS_DECIMAL_DIGIT(*pattern)) {
                it_max = REPAN_PRIV(parse_decimal)(&pattern);

                if (it_max == REPAN_DECIMAL_INF) {
                    context->error = REPAN_ERR_TOO_BIG_QUANTIFIER;
                    context->pattern = pattern;
                    return REPAN_FALSE;
                }
            }
        }

        if (context->options & REPAN_PARSE_POSIX_BASIC) {
            if (*pattern != REPAN_CHAR_BACKSLASH) {
                context->error = REPAN_ERR_BACKSLASH_EXPECTED;
                context->pattern = pattern;
                return REPAN_FALSE;
            }
            pattern++;
        }

        if (*pattern != REPAN_CHAR_RIGHT_BRACE) {
            context->error = REPAN_ERR_RIGHT_BRACE_EXPECTED;
            context->pattern = pattern;
            return REPAN_FALSE;
        }

        if (it_max < it_min) {
            context->error = REPAN_ERR_QUANTIFIER_OUT_OF_ORDER;
            return REPAN_FALSE;
        }

        pattern++;
    }

    REPAN_ASSERT(locals->last_node->type != REPAN_BRACKET_NODE
            || locals->prev_node == (repan_prev_node*)locals->last_node);
    REPAN_ASSERT(locals->last_node->type == REPAN_BRACKET_NODE
            || locals->prev_node == NULL
            || locals->prev_node->next_node == locals->last_node);

    if (locals->prev_node == NULL || locals->last_node->type > REPAN_NODE_ITERABLE_MAX) {
        context->error = REPAN_ERR_NOTHING_TO_REPEAT;
        return REPAN_FALSE;
    }

    context->pattern = pattern;

    if (it_min == 1 && it_max == 1) {
        return REPAN_TRUE;
    }

    if (locals->last_node->type != REPAN_BRACKET_NODE) {
        bracket_node = REPAN_ALLOC(repan_bracket_node, context->result);

        if (bracket_node == NULL) {
            context->error = REPAN_ERR_NO_MEMORY;
            return REPAN_FALSE;
        }

        bracket_node->next_node = NULL;
        bracket_node->type = REPAN_BRACKET_NODE;
        bracket_node->sub_type = REPAN_NON_CAPTURING_BRACKET;

        bracket_node->alt_node_list.next_node = locals->last_node;
        bracket_node->alt_node_list.next_alt_node = NULL;

        locals->prev_node->next_node = (repan_node*)bracket_node;
        locals->last_node = (repan_node*)bracket_node;
        locals->prev_node = (repan_prev_node*)bracket_node;
    }
    else {
        bracket_node = (repan_bracket_node*)locals->last_node;

        if (bracket_node->it_min != 1 || bracket_node->it_max != 1) {
            repan_bracket_node *sub_bracket_node = REPAN_ALLOC(repan_bracket_node, context->result);

            if (sub_bracket_node == NULL) {
                context->error = REPAN_ERR_NO_MEMORY;
                return REPAN_FALSE;
            }

            *sub_bracket_node = *bracket_node;

            bracket_node->sub_type = REPAN_NON_CAPTURING_BRACKET;

            bracket_node->alt_node_list.next_node = (repan_node*)sub_bracket_node;
            bracket_node->alt_node_list.next_alt_node = NULL;
        }
    }

    bracket_node->it_type = REPAN_BRACKET_IT_GREEDY;
    bracket_node->it_min = it_min;
    bracket_node->it_max = it_max;

    return REPAN_TRUE;
}

static void parse_character(repan_parser_context *context, repan_parser_locals *locals, uint32_t current_char)
{
    uint32_t *char_start = context->pattern;
    uint8_t node_type = REPAN_CHAR_NODE;
    uint8_t node_sub_type = 0;
    size_t size;
    repan_node *node;

    context->pattern++;

    switch (current_char) {
    case REPAN_CHAR_BACKSLASH:
        current_char = *context->pattern++;

        if (current_char >= REPAN_CHAR_1 && current_char <= REPAN_CHAR_9) {
            uint32_t backref_num = current_char - REPAN_CHAR_0;
            repan_reference_node *reference_node;

            if (backref_num > locals->capture_starts
                    || backref_num <= locals->capture_starts - locals->capture_ends) {
                context->error = REPAN_ERR_CAPTURING_BRACKET_NOT_EXIST;
                context->pattern -= 2;
                return;
            }

            reference_node = REPAN_ALLOC(repan_reference_node, context->result);

            if (reference_node == NULL) {
                context->error = REPAN_ERR_NO_MEMORY;
                context->pattern--;
                return;
            }

            reference_node->next_node = NULL;
            reference_node->type = REPAN_BACK_REFERENCE_NODE;
            reference_node->u.number = backref_num;

            locals->prev_node = (repan_prev_node*)locals->last_node;
            locals->last_node->next_node = (repan_node*)reference_node;
            locals->last_node = (repan_node*)reference_node;
            return;
        }
        break;
    case REPAN_CHAR_CIRCUMFLEX_ACCENT:
        node_type = REPAN_ASSERT_CIRCUMFLEX_NODE;
        node_sub_type = (uint8_t)(context->options & REPAN_PARSE_MULTILINE);
        break;
    case REPAN_CHAR_DOLLAR:
        node_type = REPAN_ASSERT_DOLLAR_NODE;
        node_sub_type = (uint8_t)(context->options & REPAN_PARSE_MULTILINE);
        break;
    case REPAN_CHAR_DOT:
        node_type = REPAN_DOT_NODE;
        node_sub_type = (uint8_t)(context->options & REPAN_PARSE_DOT_ANY);
        break;
    }

    size = (node_type == REPAN_CHAR_NODE) ? sizeof(repan_char_node) : sizeof(repan_node);
    node = (repan_node*)REPAN_PRIV(alloc)(context->result, size);

    if (node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        context->pattern = char_start;
        return;
    }

    node->next_node = NULL;
    node->type = node_type;
    node->sub_type = node_sub_type;

    if (node_type == REPAN_CHAR_NODE) {
        repan_char_node *char_node = (repan_char_node*)node;

        char_node->caseless = REPAN_FALSE;
        char_node->chr = current_char;
    }

    locals->prev_node = (repan_prev_node*)locals->last_node;
    locals->last_node->next_node = node;
    locals->last_node = node;
}

static void parse_char_range(repan_parser_context *context, repan_parser_locals *locals)
{
    repan_char_class_node *char_class_node = REPAN_ALLOC(repan_char_class_node, context->result);
    repan_prev_node *prev_node;
    uint32_t *class_start, *pattern, *pattern_end;
    int in_range = REPAN_FALSE;
    uint32_t *range_start = NULL;
    uint32_t range_left = 0;

    if (char_class_node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        return;
    }

    char_class_node->next_node = NULL;
    char_class_node->type = REPAN_CHAR_CLASS_NODE;
    char_class_node->sub_type = REPAN_NORMAL_CLASS;
    char_class_node->caseless = REPAN_FALSE;
    char_class_node->node_list.next_node = NULL;

    locals->last_node->next_node = (repan_node*)char_class_node;
    locals->last_node = (repan_node*)char_class_node;

    pattern = context->pattern + 1;
    if (*pattern == REPAN_CHAR_CIRCUMFLEX_ACCENT) {
        char_class_node->sub_type = REPAN_NEGATED_CLASS;
        pattern++;
    }

    prev_node = &char_class_node->node_list;
    class_start = context->pattern;
    pattern_end = context->pattern_end;

    do {
        uint32_t current_char;
        size_t size;
        repan_node *node;
        uint8_t node_type = REPAN_CHAR_NODE;
        uint8_t node_sub_type = 0;

        context->pattern = pattern;

        if (pattern >= pattern_end) {
            context->error = REPAN_ERR_UNTERMINATED_CHAR_CLASS;
            context->pattern = class_start;
            return;
        }

        if (!in_range) {
            range_start = pattern;
        }

        current_char = *pattern++;

        if (current_char == REPAN_CHAR_LEFT_SQUARE_BRACKET && *pattern == REPAN_CHAR_COLON) {
            uint32_t data = REPAN_PRIV(parse_posix_class)(&pattern);

            if (data != 0) {
                node_type = (data & REPAN_POSIX_OPT_IS_PERL) ? REPAN_PERL_CLASS_NODE : REPAN_POSIX_CLASS_NODE;
                node_sub_type = (uint8_t)data;
            }
        }

        size = (node_type == REPAN_CHAR_NODE) ? sizeof(repan_char_node) : sizeof(repan_node);

        if (!in_range) {
            if (pattern[0] == REPAN_CHAR_MINUS && pattern[1] != REPAN_CHAR_RIGHT_SQUARE_BRACKET) {
                if (node_type != REPAN_CHAR_NODE) {
                    context->error = REPAN_ERR_INVALID_RANGE;
                    context->pattern = range_start;
                    return;
                }

                in_range = REPAN_TRUE;
                range_left = current_char;
                pattern++;
                continue;
            }
        }
        else {
            in_range = REPAN_FALSE;

            if (node_type != REPAN_CHAR_NODE) {
                context->error = REPAN_ERR_INVALID_RANGE;
                context->pattern = range_start;
                return;
            }

            if (current_char < range_left) {
                context->error = REPAN_ERR_RANGE_OUT_OF_ORDER;
                context->pattern = range_start;
                return;
            }

            if (current_char > range_left) {
                size = sizeof(repan_char_range_node);
                node_type = REPAN_CHAR_RANGE_NODE;
            }
        }

        node = (repan_node*)REPAN_PRIV(alloc)(context->result, size);

        if (node == NULL) {
            context->error = REPAN_ERR_NO_MEMORY;
            return;
        }

        node->next_node = NULL;
        node->type = node_type;
        node->sub_type = node_sub_type;

        if (node_type == REPAN_CHAR_NODE) {
            repan_char_node *char_node = (repan_char_node*)node;

            char_node->caseless = REPAN_FALSE;
            char_node->chr = current_char;
        }
        else if (node_type == REPAN_CHAR_RANGE_NODE) {
            repan_char_range_node *char_range_node = (repan_char_range_node*)node;

            char_range_node->caseless = REPAN_FALSE;
            char_range_node->chrs[0] = range_left;
            char_range_node->chrs[1] = current_char;
        }

        prev_node->next_node = node;
        prev_node = (repan_prev_node*)node;
    } while (*pattern != REPAN_CHAR_RIGHT_SQUARE_BRACKET);

    context->pattern = pattern + 1;
}

void REPAN_PRIV(parse_posix_bracket)(repan_parser_context *context)
{
    repan_parser_locals locals;
    int is_basic = (context->options & REPAN_PARSE_POSIX_BASIC) != 0;

    REPAN_PRIV(stack_init)(&context->stack, sizeof(repan_parser_saved_bracket));

    locals.last_node = (repan_node*)&context->result->bracket_node;
    locals.capture_starts = 0;
    locals.capture_ends = 0;

    locals.bracket_type = REPAN_NON_CAPTURING_BRACKET;
    locals.current.bracket_start = context->pattern;

    while (REPAN_TRUE) {
        locals.current.bracket_node = REPAN_ALLOC(repan_bracket_node, context->result);

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

        while (REPAN_TRUE) {
            uint32_t current_char, meta_char;
            repan_alt_node *new_alt_node;

            if (context->pattern >= context->pattern_end) {
                if (!REPAN_STACK_IS_EMPTY(&context->stack)) {
                    context->error = REPAN_ERR_UNTERMINATED_BRACKET;
                    context->pattern = locals.current.bracket_start;
                }

                context->result->capture_count = locals.capture_starts;
                return;
            }

            current_char = *context->pattern;

            if (is_basic) {
                meta_char = REPAN_CHAR_NUL;
                if (current_char == REPAN_CHAR_BACKSLASH
                        && context->pattern[1] != REPAN_CHAR_ASTERISK
                        && context->pattern[1] != REPAN_CHAR_LEFT_SQUARE_BRACKET) {
                    meta_char = context->pattern[1];
                    context->pattern++;
                }
                else if (current_char == REPAN_CHAR_ASTERISK || current_char == REPAN_CHAR_LEFT_SQUARE_BRACKET) {
                    meta_char = current_char;
                }
            }
            else {
                meta_char = current_char;
            }

            switch (meta_char) {
            case REPAN_CHAR_LEFT_BRACKET:
                locals.bracket_start = context->pattern;
                locals.bracket_type = REPAN_CAPTURING_BRACKET;

                if (++locals.capture_starts > REPAN_RESOURCE_MAX) {
                    context->error = REPAN_ERR_CAPTURE_LIMIT;
                    context->pattern--;
                    return;
                }

                context->pattern++;
                goto push_new_bracket;

            case REPAN_CHAR_RIGHT_BRACKET:
                if (REPAN_STACK_IS_EMPTY(&context->stack)) {
                    context->error = REPAN_ERR_UNEXPECTED_BRACKET_CLOSE;
                    return;
                }

                locals.last_node = (repan_node*)locals.current.bracket_node;
                locals.prev_node = (repan_prev_node*)locals.current.bracket_node;
                locals.current = REPAN_STACK_TOP(repan_parser_saved_bracket, &context->stack);
                REPAN_PRIV(stack_pop)(&context->stack);

                locals.capture_ends++;
                context->pattern++;
                continue;

            case REPAN_CHAR_PIPE:
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
                continue;

            case REPAN_CHAR_LEFT_SQUARE_BRACKET:
                locals.prev_node = (repan_prev_node*)locals.last_node;
                parse_char_range(context, &locals);
                break;

            case REPAN_CHAR_ASTERISK:
            case REPAN_CHAR_PLUS:
            case REPAN_CHAR_QUESTION_MARK:
            case REPAN_CHAR_LEFT_BRACE:
                if (parse_repeat(context, &locals)) {
                    continue;
                }

                REPAN_ASSERT(context->error != REPAN_SUCCESS);
                return;

            default:
                if (is_basic && current_char == REPAN_CHAR_BACKSLASH && meta_char != REPAN_CHAR_NUL) {
                    context->pattern--;
                }

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
