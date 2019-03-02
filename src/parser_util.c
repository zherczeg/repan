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

uint32_t REPAN_PRIV(parse_decimal)(uint32_t **pattern_ref)
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

uint32_t REPAN_PRIV(parse_posix_class)(uint32_t **pattern_start)
{
    uint32_t *pattern = *pattern_start;
    uint32_t *start;
    uint32_t data;
    int negated = REPAN_FALSE;

    REPAN_ASSERT(pattern[-1] == REPAN_CHAR_LEFT_SQUARE_BRACKET && pattern[0] == REPAN_CHAR_COLON);

    pattern++;

    if (*pattern == REPAN_CHAR_CIRCUMFLEX_ACCENT) {
        negated = REPAN_TRUE;
        pattern++;
    }

    start = pattern;

    while (REPAN_IS_LOWERCASE_LATIN(*pattern)) {
        pattern++;
    }

    if (pattern[0] != REPAN_CHAR_COLON || pattern[1] != REPAN_CHAR_RIGHT_SQUARE_BRACKET) {
        return 0;
    }

    if (pattern == start) {
        return 0;
    }

    data = REPAN_PRIV(find_posix_class)(start, pattern - start);

    if (data == 0) {
        return 0;
    }

    *pattern_start = pattern + 2;

    if (negated) {
        data += (data & REPAN_POSIX_OPT_IS_PERL) ? REPAN_NEG_PERL_CLASS : REPAN_NEG_POSIX_CLASS;
    }
    return data;
}

int REPAN_PRIV(parse_repeat)(repan_parser_context *context,
    repan_node **last_node_ref, repan_prev_node **prev_node_ref)
{
    repan_bracket_node *bracket_node;
    uint32_t *pattern = context->pattern;
    repan_node *last_node = *last_node_ref;
    repan_prev_node *prev_node = *prev_node_ref;
    uint8_t it_type = REPAN_BRACKET_IT_GREEDY;
    uint32_t it_min, it_max;
    uint32_t current_char = *pattern;

    REPAN_ASSERT(last_node->next_node == NULL);

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

        it_min = REPAN_PRIV(parse_decimal)(&pattern);
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
                it_max = REPAN_PRIV(parse_decimal)(&pattern);

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

    REPAN_ASSERT(last_node->type != REPAN_BRACKET_NODE
            || prev_node == NULL
            || prev_node == (repan_prev_node*)last_node);
    REPAN_ASSERT(last_node->type == REPAN_BRACKET_NODE
            || prev_node == NULL
            || prev_node->next_node == last_node);

    if (prev_node == NULL || last_node->type > REPAN_NODE_ITERABLE_MAX) {
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
            *prev_node_ref = NULL;
            return REPAN_TRUE;
        }
    }

    if (last_node->type != REPAN_BRACKET_NODE) {
        bracket_node = REPAN_ALLOC(repan_bracket_node, context->result);

        if (bracket_node == NULL) {
            context->error = REPAN_ERR_NO_MEMORY;
            return REPAN_FALSE;
        }

        bracket_node->next_node = NULL;
        bracket_node->type = REPAN_BRACKET_NODE;
        bracket_node->sub_type = REPAN_NON_CAPTURING_BRACKET;

        bracket_node->alt_node_list.next_node = last_node;
        bracket_node->alt_node_list.next_alt_node = NULL;

        prev_node->next_node = (repan_node*)bracket_node;
        *last_node_ref = (repan_node*)bracket_node;
    }
    else {
        bracket_node = (repan_bracket_node*)last_node;
    }

    bracket_node->it_type = it_type;
    bracket_node->it_min = it_min;
    bracket_node->it_max = it_max;
    *prev_node_ref = NULL;

    return REPAN_TRUE;
}
