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
    uint8_t caseless;
} repan_expand_saved_bracket;

typedef struct {
    repan_pattern *pattern;
    repan_stack stack;
    uint32_t options;
} repan_expand_context;

static repan_node *expand_script_ext(repan_expand_context *context,
    const uint32_t *property, repan_u_property_node *property_node, repan_prev_node *prev_node, uint8_t caseless)
{
    repan_char_class_node *class_node = REPAN_ALLOC(repan_char_class_node, context->pattern);
    const uint32_t *property_end;

    if (class_node == NULL) {
        return NULL;
    }

    prev_node->next_node = (repan_node*)class_node;

    class_node->next_node = property_node->next_node;
    class_node->type = REPAN_CHAR_CLASS_NODE;
    class_node->sub_type = REPAN_NORMAL_CLASS;
    class_node->caseless = caseless;

    if (property_node->sub_type & REPAN_U_NEGATED_PROPERTY) {
        class_node->sub_type = REPAN_NEGATED_CLASS;
        property_node->sub_type = (uint8_t)(property_node->sub_type & ~REPAN_U_NEGATED_PROPERTY);
    }

    class_node->node_list.next_node = (repan_node*)property_node;
    property_node->next_node = NULL;

    prev_node = (repan_prev_node*)property_node;
    property += 2;
    property_end = property + property[-1];

    do {
        if (*property & REPAN_RANGE_START) {
            repan_char_range_node *char_range_node = REPAN_ALLOC(repan_char_range_node, context->pattern);

            char_range_node->next_node = NULL;
            char_range_node->type = REPAN_CHAR_RANGE_NODE;
            char_range_node->caseless = caseless;
            char_range_node->chrs[0] = *property++ & ~REPAN_RANGE_START;
            char_range_node->chrs[1] = *property;

            prev_node->next_node = (repan_node*)char_range_node;
            prev_node = (repan_prev_node*)char_range_node;
        }
        else {
            repan_char_node *char_node = REPAN_ALLOC(repan_char_node, context->pattern);

            char_node->next_node = NULL;
            char_node->type = REPAN_CHAR_NODE;
            char_node->caseless = caseless;
            char_node->chr = *property;

            prev_node->next_node = (repan_node*)char_node;
            prev_node = (repan_prev_node*)char_node;
        }

        property++;
    } while (property < property_end);

    return (repan_node*)class_node;
}

static uint32_t expand(repan_expand_context *context, repan_bracket_node *bracket_node)
{
    repan_expand_saved_bracket current;
    repan_node *node;
    repan_prev_node *prev_node;

    current.bracket_node = bracket_node;
    current.alt_node = &bracket_node->alt_node_list;
    current.caseless = (uint8_t)(context->pattern->options & REPAN_PARSE_CASELESS);
    prev_node = (repan_prev_node*)current.alt_node;
    node = prev_node->next_node;

    while (REPAN_TRUE) {
        while (node != NULL) {
            repan_bracket_node *bracket_node;
            repan_u_property_node *property_node;
            const uint32_t *property;

            switch (node->type) {
            case REPAN_BRACKET_NODE:
                if (!REPAN_PRIV(stack_push)(&context->stack)) {
                    return REPAN_ERR_NO_MEMORY;
                }

                *REPAN_STACK_TOP_PTR(repan_expand_saved_bracket, &context->stack) = current;

                bracket_node = (repan_bracket_node*)node;
                current.bracket_node = bracket_node;
                current.alt_node = &bracket_node->alt_node_list;
                prev_node = (repan_prev_node*)current.alt_node;
                node = prev_node->next_node;
                continue;

            case REPAN_U_PROPERTY_NODE:
                property_node = (repan_u_property_node*)node;
                property = REPAN_U_GET_PROPERTY_PTR(property_node->property);

                if ((property[0] & REPAN_U_DEFINE_SCRIPT) == REPAN_U_DEFINE_SCRIPT) {
                    if (!(property_node->sub_type & REPAN_U_SCRIPT_EXTENSION)
                            || !(context->options & REPAN_UNICODE_EXPAND_SCRIPT_EXT)) {
                        break;
                    }

                    property_node->sub_type = (uint8_t)(property_node->sub_type & ~REPAN_U_SCRIPT_EXTENSION);

                    if (property[1] == 0) {
                        break;
                    }

                    node = expand_script_ext(context, property, property_node, prev_node, current.caseless);

                    if (node == NULL) {
                        return REPAN_ERR_NO_MEMORY;
                    }
                    break;
                }
                break;

            case REPAN_CHAR_NODE:
                current.caseless = ((repan_char_node*)node)->caseless;
                break;

            case REPAN_CHAR_CLASS_NODE:
                current.caseless = ((repan_char_class_node*)node)->caseless;
                break;
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

        current = REPAN_STACK_TOP(repan_expand_saved_bracket, &context->stack);
        REPAN_PRIV(stack_pop)(&context->stack);
    }

    return REPAN_SUCCESS;
}

uint32_t repan_expand(repan_pattern *pattern, uint32_t options)
{
    repan_expand_context context;
    uint32_t result;

    if (pattern->options & REPAN_PATTERN_DAMAGED) {
        return REPAN_ERR_DAMAGED_PATTERN;
    }

    if (options == 0) {
        return REPAN_SUCCESS;
    }

    REPAN_PRIV(stack_init)(&context.stack, sizeof(repan_expand_saved_bracket));
    context.pattern = pattern;
    context.options = options;

    result = expand(&context, pattern->bracket_node);

    if (result != REPAN_SUCCESS) {
        pattern->options |= REPAN_PATTERN_DAMAGED;
    }

    REPAN_PRIV(stack_free)(&context.stack);
    return result;
}
