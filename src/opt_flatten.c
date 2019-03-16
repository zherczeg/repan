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
} repan_flatten_saved_bracket;

typedef struct {
    repan_pattern *pattern;
    repan_stack stack;
} repan_flatten_context;

static void remove_alternatives(repan_flatten_context *context, repan_bracket_node *bracket_node)
{
    repan_alt_node *prev_alt_node = &bracket_node->alt_node_list;
    int was_empty = (prev_alt_node->next_node == NULL);
    repan_alt_node *alt_node = prev_alt_node->next_alt_node;

    while (alt_node != NULL) {
        if (alt_node->next_node == NULL) {
            if (was_empty) {
                repan_alt_node *next_alt_node = alt_node->next_alt_node;

                REPAN_PRIV(free)(context->pattern, alt_node, sizeof(repan_alt_node));

                prev_alt_node->next_alt_node = next_alt_node;
                alt_node = next_alt_node;
                continue;
            }

            was_empty = REPAN_TRUE;
        }
        else {
            was_empty = REPAN_FALSE;
        }

        prev_alt_node = alt_node;
        alt_node = alt_node->next_alt_node;
    }
}

static uint32_t flatten(repan_flatten_context *context, repan_bracket_node *bracket_node)
{
    repan_flatten_saved_bracket current;
    repan_node *node;
    repan_prev_node *prev_node;

    current.bracket_node = bracket_node;
    current.alt_node = &bracket_node->alt_node_list;
    prev_node = (repan_prev_node*)current.alt_node;
    node = prev_node->next_node;

    while (REPAN_TRUE) {
        while (node != NULL) {
            repan_bracket_node *bracket_node;

            if (node->type == REPAN_BRACKET_NODE) {
                bracket_node = (repan_bracket_node*)node;

                remove_alternatives(context, bracket_node);

                if (bracket_node->sub_type == REPAN_NON_CAPTURING_BRACKET
                        && bracket_node->alt_node_list.next_alt_node == NULL
                        && bracket_node->it_min == 1 && bracket_node->it_max == 1) {
                    repan_node *last_node = bracket_node->alt_node_list.next_node;

                    if (last_node != NULL) {
                        while (last_node->next_node != NULL) {
                            last_node = last_node->next_node;
                        }

                        last_node->next_node = bracket_node->next_node;
                        node = bracket_node->alt_node_list.next_node;
                    }
                    else {
                        node = bracket_node->next_node;
                    }

                    prev_node->next_node = node;
                    REPAN_PRIV(free)(context->pattern, bracket_node, sizeof(repan_bracket_node));
                    continue;
                }

                if (!REPAN_PRIV(stack_push)(&context->stack)) {
                    return REPAN_ERR_NO_MEMORY;
                }

                *REPAN_STACK_TOP_PTR(repan_flatten_saved_bracket, &context->stack) = current;

                current.bracket_node = bracket_node;
                current.alt_node = &bracket_node->alt_node_list;
                prev_node = (repan_prev_node*)current.alt_node;
                node = prev_node->next_node;
                continue;
            }

            prev_node = (repan_prev_node*)node;
            node = node->next_node;
        }

        current.alt_node = current.alt_node->next_alt_node;
        if (current.alt_node != NULL) {
            prev_node = (repan_prev_node*)current.alt_node;
            node = prev_node->next_node;
            continue;
        }

        if (REPAN_STACK_IS_EMPTY(&context->stack)) {
            break;
        }

        prev_node = (repan_prev_node*)current.bracket_node;
        node = prev_node->next_node;

        current = REPAN_STACK_TOP(repan_flatten_saved_bracket, &context->stack);
        REPAN_PRIV(stack_pop)(&context->stack);
    }

    return REPAN_SUCCESS;
}

uint32_t repan_opt_flatten(repan_pattern *pattern, uint32_t options)
{
    repan_flatten_context context;
    uint32_t result;

    if (pattern->options & REPAN_PATTERN_DAMAGED) {
        return REPAN_ERR_DAMAGED_PATTERN;
    }

    REPAN_PRIV(stack_init)(&context.stack, sizeof(repan_flatten_saved_bracket));
    context.pattern = pattern;

    result = flatten(&context, pattern->bracket_node);

    if (result != REPAN_SUCCESS) {
        pattern->options |= REPAN_PATTERN_DAMAGED;
    }

    REPAN_PRIV(stack_free)(&context.stack);
    return result;
}
