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
    uint32_t capture_reset;
    uint32_t capture_max;
} repan_uncapture_saved_bracket;

typedef struct {
    uint32_t *capture_idx;
    repan_pattern *pattern;
    repan_stack stack;
} repan_uncapture_context;

static repan_node *to_normal_bracket(repan_node *node, repan_prev_node *prev_node, repan_pattern *pattern)
{
    repan_bracket_node *bracket_node = REPAN_ALLOC(repan_bracket_node, pattern);

    if (bracket_node == NULL) {
        return NULL;
    }

    *bracket_node = ((repan_ext_bracket_node*)node)->bracket_node;
    prev_node->next_node = (repan_node*)bracket_node;

    REPAN_PRIV(free)(pattern, node, sizeof(repan_ext_bracket_node));

    return (repan_node*)bracket_node;
}

static uint32_t uncapture(repan_uncapture_context *context, repan_bracket_node *bracket_node, int analyze)
{
    repan_uncapture_saved_bracket current;
    uint32_t capture_count = 0;
    repan_node *node;
    repan_prev_node *prev_node;

    current.bracket_node = bracket_node;
    current.alt_node = &bracket_node->alt_node_list;
    prev_node = (repan_prev_node*)current.alt_node;
    node = prev_node->next_node;

    while (REPAN_TRUE) {
        while (node != NULL) {
            uint32_t number = 0;
            repan_string *name;

            REPAN_ASSERT(prev_node != NULL && prev_node->next_node == node);

            switch(node->type) {
            case REPAN_BRACKET_NODE:
                if (!REPAN_PRIV(stack_push)(&context->stack)) {
                    return REPAN_ERR_NO_MEMORY;
                }

                *REPAN_STACK_TOP_PTR(repan_uncapture_saved_bracket, &context->stack) = current;

                current.bracket_node = (repan_bracket_node*)node;

                switch (current.bracket_node->sub_type) {
                case REPAN_CAPTURING_BRACKET:
                    if (!analyze && context->capture_idx[capture_count] == 0) {
                        current.bracket_node->sub_type = REPAN_NON_CAPTURING_BRACKET;
                    }
                    capture_count++;
                    break;
                case REPAN_NAMED_CAPTURING_BRACKET:
                    name = ((repan_ext_bracket_node*)current.bracket_node)->u.name;
                    REPAN_ASSERT(name != NULL && name->length_and_flags & REPAN_STRING_DEFINED);

                    if (analyze) {
                        if (name->length_and_flags & REPAN_STRING_REFERENCED) {
                            context->capture_idx[capture_count] = 1;
                        }
                    }
                    else if (context->capture_idx[capture_count] == 0) {
                        node = to_normal_bracket(node, prev_node, context->pattern);
                        node->sub_type = REPAN_NON_CAPTURING_BRACKET;
                        current.bracket_node = (repan_bracket_node*)node;
                    }

                    capture_count++;
                    break;
                case REPAN_CAPTURE_RESET_BRACKET:
                    current.capture_reset = capture_count;
                    current.capture_max = capture_count;
                    break;
                case REPAN_COND_REFERENCE_BRACKET:
                case REPAN_COND_RECURSE_BRACKET:
                    number = ((repan_ext_bracket_node*)current.bracket_node)->u.number;
                    REPAN_ASSERT(number <= context->pattern->capture_count);

                    if (number == 0) {
                        break;
                    }

                    if (analyze) {
                        context->capture_idx[number - 1] = 1;
                        break;
                    }

                    number = context->capture_idx[number - 1];
                    ((repan_ext_bracket_node*)current.bracket_node)->u.number = number;
                    break;
                }

                current.alt_node = &current.bracket_node->alt_node_list;
                prev_node = (repan_prev_node*)current.alt_node;
                node = prev_node->next_node;
                continue;
            case REPAN_BACK_REFERENCE_NODE:
            case REPAN_RECURSE_NODE:
                number = ((repan_reference_node*)node)->u.number;
                REPAN_ASSERT(number <= context->pattern->capture_count);

                if (number == 0) {
                    break;
                }

                if (analyze) {
                    context->capture_idx[number - 1] = 1;
                    break;
                }

                number = context->capture_idx[number - 1];
                ((repan_reference_node*)node)->u.number = number;
                break;
            }

            prev_node = (repan_prev_node*)node;
            node = node->next_node;
        }

        current.alt_node = current.alt_node->next_alt_node;
        if (current.alt_node != NULL) {
            prev_node = (repan_prev_node*)current.alt_node;
            node = prev_node->next_node;

            if (current.bracket_node->sub_type == REPAN_CAPTURE_RESET_BRACKET) {
                if (capture_count > current.capture_max) {
                    current.capture_max = capture_count;
                }
                capture_count = current.capture_reset;
            }
            continue;
        }

        prev_node = (repan_prev_node*)current.bracket_node;
        node = prev_node->next_node;

        if (current.bracket_node->sub_type == REPAN_CAPTURE_RESET_BRACKET
                && capture_count < current.capture_max) {
            capture_count = current.capture_max;
        }

        if (REPAN_STACK_IS_EMPTY(&context->stack)) {
            break;
        }

        current = REPAN_STACK_TOP(repan_uncapture_saved_bracket, &context->stack);
        REPAN_PRIV(stack_pop)(&context->stack);
    }

    REPAN_ASSERT(capture_count == context->pattern->capture_count);
    return REPAN_SUCCESS;
}

uint32_t repan_uncapture(repan_pattern *pattern)
{
    repan_uncapture_context context;
    uint32_t i, count, result;

    if (pattern->capture_count == 0) {
        return REPAN_SUCCESS;
    }

    REPAN_PRIV(stack_init)(&context.stack, sizeof(repan_uncapture_saved_bracket));
    context.capture_idx = (uint32_t*)calloc(pattern->capture_count, sizeof(uint32_t));
    context.pattern = pattern;

    if (context.capture_idx == NULL) {
        return REPAN_ERR_NO_MEMORY;
    }

    result = uncapture(&context, pattern->bracket_node, REPAN_TRUE);

    if (result == REPAN_SUCCESS) {
        count = 0;
        for (i = 0; i < pattern->capture_count; i++) {
            if (context.capture_idx[i] != 0) {
                context.capture_idx[i] = ++count;
            }
        }

        result = uncapture(&context, pattern->bracket_node, REPAN_FALSE);
        if (result == REPAN_SUCCESS) {
            pattern->capture_count = count;
        }
    }

    free(context.capture_idx);
    REPAN_PRIV(stack_free)(&context.stack);

    return result;
}
