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

typedef struct {
    repan_bracket_node *bracket_node;
    repan_alt_node *alt_node;
    repan_prev_node *prev_node;
} repan_merge_prefix_saved_bracket;

typedef struct {
    repan_pattern *pattern;
    repan_stack stack;
} repan_merge_prefix_context;

static uint32_t try_merge(repan_merge_prefix_context *context,
    repan_alt_node *start_alt_node, repan_char_node *char_node)
{
    uint32_t chr;
    uint32_t tmp_buf[REPAN_OTHER_CASE_TMP_BUF_SIZE];
    const uint32_t *other_cases = NULL;

    repan_alt_node *alt_node, *prev_alt_node;
    repan_alt_node *merge_prev_alt_node;

    REPAN_ASSERT(start_alt_node->next_node == (repan_node*)char_node);
    REPAN_ASSERT(char_node->type == REPAN_CHAR_NODE);

    chr = char_node->chr;

    if (char_node->caseless) {
        other_cases = REPAN_PRIV(get_other_cases)(context->pattern, chr, tmp_buf);
    }

    prev_alt_node = start_alt_node;
    alt_node = start_alt_node->next_alt_node;
    merge_prev_alt_node = NULL;

    while (alt_node != NULL) {
        repan_node *node = alt_node->next_node;
        repan_char_node *current_char_node;
        const uint32_t *current_other_cases = NULL;
        uint32_t current_chr;

        if (node == NULL || node->type != REPAN_CHAR_NODE) {
            break;
        }

        current_char_node = (repan_char_node*)node;
        current_chr = current_char_node->chr;

        if (other_cases != NULL) {
            current_other_cases = other_cases;
        }
        else if (current_char_node->caseless) {
            current_other_cases = REPAN_PRIV(get_other_cases)(context->pattern, current_chr, tmp_buf);
            if (current_other_cases != NULL) {
                current_chr = chr;
            }
        }

        if (current_other_cases != NULL) {
            do {
                if (*current_other_cases == current_chr) {
                    current_other_cases = NULL;
                    break;
                }
                current_other_cases++;
            } while (*current_other_cases != 0);

            if (current_other_cases == NULL) {
                if (other_cases == NULL || !current_char_node->caseless) {
                    break;
                }
            }
            else {
                prev_alt_node = alt_node;
                alt_node = alt_node->next_alt_node;
                continue;
            }
        }
        else if (current_chr != chr) {
            prev_alt_node = alt_node;
            alt_node = alt_node->next_alt_node;
            continue;
        }

        if (merge_prev_alt_node == NULL) {
            repan_bracket_node *bracket_node = REPAN_ALLOC(repan_bracket_node, context->pattern);

            if (bracket_node == NULL) {
                return REPAN_ERR_NO_MEMORY;
            }

            bracket_node->next_node = NULL;
            bracket_node->type = REPAN_BRACKET_NODE;
            bracket_node->sub_type = REPAN_MERGE_ALTERNATIVES_BRACKET;
            bracket_node->it_type = REPAN_BRACKET_IT_GREEDY;
            bracket_node->it_min = 1;
            bracket_node->it_max = 1;
            bracket_node->alt_node_list.next_node = char_node->next_node;

            char_node->next_node = (repan_node*)bracket_node;
            merge_prev_alt_node = &bracket_node->alt_node_list;
        }

        alt_node->next_node = current_char_node->next_node;
        REPAN_PRIV(free)(context->pattern, current_char_node, sizeof(repan_char_node));

        merge_prev_alt_node->next_alt_node = alt_node;
        merge_prev_alt_node = alt_node;

        alt_node = alt_node->next_alt_node;
        prev_alt_node->next_alt_node = alt_node;

        merge_prev_alt_node->next_alt_node = NULL;
    }

    return REPAN_SUCCESS;
}

static uint32_t merge_alternatives(repan_merge_prefix_context *context, repan_bracket_node *bracket_node)
{
    repan_merge_prefix_saved_bracket current;
    repan_node *node;

    current.bracket_node = bracket_node;
    current.alt_node = &bracket_node->alt_node_list;
    current.prev_node = (repan_prev_node*)current.alt_node;
    node = current.prev_node->next_node;

    while (REPAN_TRUE) {
        repan_bracket_node *bracket_node;

        if (current.prev_node == (repan_prev_node*)current.alt_node
                && node != NULL && node->type == REPAN_CHAR_NODE) {
            uint32_t result = try_merge(context, current.alt_node, (repan_char_node*)node);

            if (result != REPAN_SUCCESS) {
                return result;
            }
        }

        while (node != NULL && node->type != REPAN_BRACKET_NODE) {
            current.prev_node = (repan_prev_node*)node;
            node = node->next_node;
        }

        if (node != NULL) {
            repan_bracket_node *bracket_node = (repan_bracket_node*)node;

            REPAN_ASSERT(node->type == REPAN_BRACKET_NODE);

            if (!REPAN_PRIV(stack_push)(&context->stack)) {
                return REPAN_ERR_NO_MEMORY;
            }

            *REPAN_STACK_TOP_PTR(repan_merge_prefix_saved_bracket, &context->stack) = current;

            current.bracket_node = bracket_node;
            current.alt_node = &bracket_node->alt_node_list;
            current.prev_node = (repan_prev_node*)current.alt_node;
            node = current.prev_node->next_node;
            continue;
        }

        current.alt_node = current.alt_node->next_alt_node;
        if (current.alt_node != NULL) {
            current.prev_node = (repan_prev_node*)current.alt_node;
            node = current.prev_node->next_node;
            continue;
        }

        if (REPAN_STACK_IS_EMPTY(&context->stack)) {
            REPAN_ASSERT(current.bracket_node->sub_type != REPAN_MERGE_ALTERNATIVES_BRACKET);
            break;
        }

        bracket_node = current.bracket_node;

        current = REPAN_STACK_TOP(repan_merge_prefix_saved_bracket, &context->stack);
        REPAN_PRIV(stack_pop)(&context->stack);

        REPAN_ASSERT(current.prev_node->next_node == (repan_node*)bracket_node);

        if (bracket_node->sub_type == REPAN_MERGE_ALTERNATIVES_BRACKET) {
            REPAN_ASSERT(bracket_node->next_node == NULL);

            if (bracket_node->alt_node_list.next_alt_node == NULL) {
                node = bracket_node->alt_node_list.next_node;
                current.prev_node->next_node = node;
                REPAN_PRIV(free)(context->pattern, bracket_node, sizeof(repan_bracket_node));
                continue;
            }

            bracket_node->sub_type = REPAN_NON_CAPTURING_BRACKET;
        }

        current.prev_node = (repan_prev_node*)bracket_node;
        node = bracket_node->next_node;
    }

    return REPAN_SUCCESS;
}

uint32_t repan_opt_merge_alternatives(repan_pattern *pattern, uint32_t options)
{
    repan_merge_prefix_context context;
    uint32_t result;

    if (pattern->options & REPAN_PATTERN_DAMAGED) {
        return REPAN_ERR_DAMAGED_PATTERN;
    }

    REPAN_PRIV(stack_init)(&context.stack, sizeof(repan_merge_prefix_saved_bracket));
    context.pattern = pattern;

    result = merge_alternatives(&context, pattern->bracket_node);

    if (result != REPAN_SUCCESS) {
        pattern->options |= REPAN_PATTERN_DAMAGED;
    }

    REPAN_PRIV(stack_free)(&context.stack);
    return result;
}
