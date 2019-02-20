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

enum {
    REPAN_BRACKET_CMD_NONE,
    REPAN_BRACKET_CMD_READ_GROUP_NAME,
};

typedef struct {
    repan_parser_saved_bracket current;
    repan_node *last_node;
    uint32_t capture_count;

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
} repan_parser_locals;

static uint32_t parse_decimal(uint32_t **pattern_ref)
{
    uint32_t *pattern = *pattern_ref;
    uint32_t decimal = 0;
    uint32_t digit = *pattern++;

    do {
        if (decimal >= 100000000) {
            return REPAN_DECIMAL_INF;
        }

        decimal = decimal * 10 + digit - REPAN_CHAR_0;
        digit = *pattern++;
    } while (REPAN_IS_DECIMAL_DIGIT(digit));

    *pattern_ref = pattern - 1;
    return decimal;
}

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

#include "parser_javascript_char_inl.c"

static int parse_repeat(repan_parser_context *context, repan_parser_locals *locals, uint32_t current_char)
{
    repan_bracket_node *bracket_node;
    uint32_t *pattern = context->pattern;
    uint8_t it_type = REPAN_BRACKET_IT_GREEDY;
    uint32_t it_min;
    uint32_t it_max;

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

        if (!REPAN_IS_DECIMAL_DIGIT(*pattern)) {
            return REPAN_FALSE;
        }

        it_min = parse_decimal(&pattern);
        if (it_min == REPAN_DECIMAL_INF) {
            context->error = REPAN_ERR_TOO_BIG_QUANTIFIER;
            context->pattern = pattern;
            return REPAN_FALSE;
        }

        it_max = it_min;
        if (*pattern == REPAN_CHAR_COMMA) {
            pattern++;
            it_max = REPAN_DECIMAL_INF;

            if (REPAN_IS_DECIMAL_DIGIT(*pattern)) {
                it_max = parse_decimal(&pattern);
                if (it_max == REPAN_DECIMAL_INF) {
                    context->error = REPAN_ERR_TOO_BIG_QUANTIFIER;
                    context->pattern = pattern;
                    return REPAN_FALSE;
                }

            }
        }

        if (*pattern != REPAN_CHAR_RIGHT_BRACE) {
            return REPAN_FALSE;
        }

        if (it_max < it_min) {
            context->error = REPAN_ERR_QUANTIFIER_OUT_OF_ORDER;
            return REPAN_FALSE;
        }

        pattern++;
    }

    REPAN_ASSERT(locals->last_node->type != REPAN_BRACKET_NODE
            || locals->prev_node == NULL
            || locals->prev_node == (repan_prev_node*)locals->last_node);
    REPAN_ASSERT(locals->last_node->type == REPAN_BRACKET_NODE
            || locals->prev_node == NULL
            || locals->prev_node->next_node == locals->last_node);

    if (locals->prev_node == NULL || locals->last_node->type > REPAN_NODE_ITERABLE_MAX) {
        context->error = REPAN_ERR_NOTHING_TO_REPEAT;
        return REPAN_FALSE;
    }

    current_char = *pattern;
    if (current_char == REPAN_CHAR_QUESTION_MARK) {
        it_type = REPAN_BRACKET_IT_NON_GREEDY;
        pattern++;
    }
    else if (current_char == REPAN_CHAR_PLUS) {
        it_type = REPAN_BRACKET_IT_POSSESSIVE;
        pattern++;
    }

    context->pattern = pattern;

    if (it_min == it_max) {
        it_type = REPAN_BRACKET_IT_GREEDY;

        if (it_min == 1) {
            locals->prev_node = NULL;
            return REPAN_TRUE;
        }
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
    }
    else {
        bracket_node = (repan_bracket_node*)locals->last_node;
    }

    bracket_node->it_type = it_type;
    bracket_node->it_min = it_min;
    bracket_node->it_max = it_max;
    locals->prev_node = NULL;

    return REPAN_TRUE;
}

static void parse_open_bracket(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t *pattern;

    locals->bracket_size = (uint16_t)sizeof(repan_bracket_node);
    locals->bracket_type = REPAN_NON_CAPTURING_BRACKET;
    locals->bracket_command = REPAN_BRACKET_CMD_NONE;

    if (*context->pattern != REPAN_CHAR_QUESTION_MARK) {
        locals->bracket_type = REPAN_CAPTURING_BRACKET;

        if (++locals->capture_count > REPAN_RESOURCE_MAX) {
            context->error = REPAN_ERR_CAPTURE_LIMIT;
            context->pattern--;
            return;
        }
        return;
    }

    pattern = context->pattern + 2;

    switch (pattern[-1]) {
    case REPAN_CHAR_COLON:
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
        locals->bracket_size = (uint16_t)sizeof(repan_ext_bracket_node);
        locals->bracket_type = REPAN_NAMED_CAPTURING_BRACKET;
        locals->bracket_command = REPAN_BRACKET_CMD_READ_GROUP_NAME;
        context->pattern = pattern;

        if (++locals->capture_count > REPAN_RESOURCE_MAX) {
            context->error = REPAN_ERR_CAPTURE_LIMIT;
            context->pattern--;
            return;
        }
        return;
    }

    context->error = REPAN_ERR_INVALID_BRACKET;
    context->pattern--;
    return;
}

enum {
    REPAN_CONDITION_NAME,
    REPAN_CONDITION_NUMBER,
    REPAN_CONDITION_NON_ZERO_NUMBER,
};

static void parse_group_name(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t *pattern = context->pattern;
    repan_string *name;

    ((repan_ext_bracket_node*)locals->current.bracket_node)->u.name = NULL;

    if (*pattern == REPAN_CHAR_GREATER_THAN_SIGN
            || REPAN_IS_DECIMAL_DIGIT(*pattern)) {
        context->error = REPAN_ERR_NAME_EXPECTED;
        return;
    }

    while (REPAN_PRIV(is_word_char)(*pattern)) {
        pattern++;
    }

    if (*pattern != REPAN_CHAR_GREATER_THAN_SIGN) {
        context->error = REPAN_ERR_GREATER_THAN_SIGN_EXPECTED;
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

static void parse_bracket(repan_parser_context *context)
{
    repan_parser_locals locals;

    locals.last_node = (repan_node*)&context->result->bracket_node;
    locals.capture_count = 0;

    locals.bracket_size = (uint16_t)sizeof(repan_bracket_node);
    locals.bracket_type = REPAN_NON_CAPTURING_BRACKET;
    locals.bracket_command = REPAN_BRACKET_CMD_NONE;

    locals.current.bracket_start = context->pattern;

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
            REPAN_ASSERT(locals.bracket_command == REPAN_BRACKET_CMD_READ_GROUP_NAME);

            parse_group_name(context, &locals);

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

                locals.last_node = (repan_node*)locals.current.bracket_node;
                locals.prev_node = (repan_prev_node*)locals.current.bracket_node;
                locals.current = REPAN_STACK_TOP(repan_parser_saved_bracket, &context->stack);
                REPAN_PRIV(stack_pop)(&context->stack);

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
                if (parse_repeat(context, &locals, current_char)) {
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

void REPAN_PRIV(repan_parse_javascript)(repan_parser_context *context)
{
    repan_undefined_name *undefined_name;
    int undefined_found;

    context->error = REPAN_SUCCESS;
    context->pattern = context->pattern_start;
    context->undefined_names = NULL;

    context->result = (repan_pattern*)malloc(sizeof(repan_pattern));

    if (context->result == NULL) {
        free(context->pattern_start);
        context->error = REPAN_ERR_NO_MEMORY;
        return;
    }

    REPAN_PRIV(stack_init)(&context->stack, sizeof(repan_parser_saved_bracket));

    memset(context->result, 0, sizeof(repan_pattern));

    parse_bracket(context);

    free(context->pattern_start);
    REPAN_PRIV(stack_free)(&context->stack);

    undefined_name = context->undefined_names;
    undefined_found = REPAN_FALSE;

    while (undefined_name != NULL) {
        repan_undefined_name *next = undefined_name->next;

        if (!undefined_found && !(undefined_name->name->length_and_flags & REPAN_STRING_DEFINED)) {
            undefined_found = REPAN_TRUE;

            if (context->error == REPAN_SUCCESS) {
                context->error = REPAN_ERR_CAPTURING_BRACKET_NOT_EXIST;
                context->pattern = undefined_name->start;
            }
        }

        REPAN_PRIV(free)(context->result, undefined_name, sizeof(repan_undefined_name));
        undefined_name = next;
    }

    if (context->error != REPAN_SUCCESS) {
        repan_pattern_free(context->result);
        context->result = NULL;
    }
}
