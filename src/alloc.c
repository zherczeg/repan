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

#define MAX_BLOCK_COUNT 8

static void free_range(repan_node *node)
{
    while (node != NULL) {
        repan_node *next_node = node->next_node;

        REPAN_ASSERT(node->type < REPAN_CHAR_CLASS_NODE);

        free(node);
        node = next_node;
    }
}

void repan_pattern_free(repan_pattern *pattern)
{
    repan_bracket_node *bracket_node_list = pattern->bracket_node;
    int idx;

    REPAN_ASSERT(bracket_node_list->next_node == NULL);

    do {
        repan_bracket_node *bracket_node = bracket_node_list;
        repan_alt_node *alt_node = bracket_node->alt_node_list.next_alt_node;
        repan_node *node = bracket_node->alt_node_list.next_node;

        bracket_node_list = (repan_bracket_node*)bracket_node->next_node;
        free(bracket_node);

        while (REPAN_TRUE) {
            repan_alt_node *next_alt_node;

            while (node != NULL) {
                repan_node *next_node = node->next_node;

                switch (node->type) {
                case REPAN_BRACKET_NODE:
                    node->next_node = (repan_node*)bracket_node_list;
                    bracket_node_list = (repan_bracket_node*)node;
                    break;

                case REPAN_CHAR_CLASS_NODE:
                    free_range(((repan_char_class_node*)node)->node_list.next_node);
                    /* Fallthrough. */

                default:
                    free(node);
                }

                node = next_node;
            }

            if (alt_node == NULL) {
                break;
            }

            next_alt_node = alt_node->next_alt_node;
            node = alt_node->next_node;

            free(alt_node);
            alt_node = next_alt_node;
        }
    } while (bracket_node_list != NULL);

    for (idx = 0; idx < 2; idx++) {
        repan_string *current = (idx == 0) ? pattern->bracket_names : pattern->verb_arguments;

        while (current != NULL) {
            repan_string *next = current->next;
            free(current);
            current = next;
        }
    }

    for (idx = 0; idx < REPAN_RESERVED_BLOCK_COUNT; idx++) {
        repan_block *block = pattern->blocks[idx];

        while (block != NULL) {
            repan_block *next_block = block->next;
            free(block);
            block = next_block;
        }
    }

    free(pattern);
}

static const uint8_t repan_block_sizes[REPAN_RESERVED_BLOCK_COUNT] = {
    sizeof(repan_block),
    sizeof(repan_block) + sizeof(void*),
};

void *REPAN_PRIV(alloc)(repan_pattern *pattern, size_t block_size)
{
    void *result;
    int idx;

    for (idx = 0; idx < REPAN_RESERVED_BLOCK_COUNT; idx++) {
        if (block_size <= repan_block_sizes[idx]) {
            block_size = repan_block_sizes[idx];

            if (pattern->blocks[idx] != NULL) {
                result = pattern->blocks[idx];
                pattern->blocks[idx] = ((repan_block*)result)->next;
                return result;
            }
            break;
        }
    }

    return malloc(block_size);
}

void REPAN_PRIV(free)(repan_pattern *pattern, void *block, size_t block_size)
{
    int idx;

    for (idx = 0; idx < REPAN_RESERVED_BLOCK_COUNT; idx++) {
        if (block_size <= repan_block_sizes[idx]) {
            repan_block *new_block = (repan_block *)block;
            repan_block *blocks = pattern->blocks[idx];
            int count = 0;

            if (blocks != NULL) {
                count = blocks->u.count + 1;
            }

            if (count >= MAX_BLOCK_COUNT) {
                break;
            }

            new_block->next = blocks;
            new_block->u.count = count;
            pattern->blocks[idx] = new_block;
            return;
        }
    }

    free(block);
}

repan_string *REPAN_PRIV(string_add)(repan_string **strings, uint32_t *start, size_t length, uint32_t *error)
{
    repan_string *current = *strings;
    uint32_t next_index;
    size_t byte_length;

    REPAN_ASSERT(length > 0);

    if (length >= REPAN_RESOURCE_MAX) {
        *error = REPAN_ERR_LENGTH_LIMIT;
        return NULL;
    }

    byte_length = length * sizeof(uint32_t);

    while (current != NULL) {
        uint32_t current_length = current->length_and_flags >> REPAN_STRING_LENGTH_SHIFT;

        if (current_length == length && memcmp(start, current->string, byte_length) == 0) {
            /* String is already in the table. */
            return current;
        }
        current = current->next;
    }

    next_index = 0;
    current = *strings;

    if (current != NULL) {
        next_index = current->index + 1;

        if (next_index >= REPAN_RESOURCE_MAX) {
            *error = REPAN_ERR_LIST_LIMIT;
            return NULL;
        }
    }

    current = (repan_string*)malloc(sizeof(repan_string) + byte_length - sizeof(uint32_t));
    if (current == NULL) {
        *error = REPAN_ERR_NO_MEMORY;
        return NULL;
    }

    current->next = *strings;
    current->length_and_flags = (length << REPAN_STRING_LENGTH_SHIFT);
    current->index = next_index;
    memcpy(current->string, start, byte_length);

    *strings = current;
    return current;
}

#define REPAN_STACK_GROWTH 8

void REPAN_PRIV(stack_init)(repan_stack *stack, size_t type_size)
{
    stack->first = NULL;
    stack->free_block = NULL;

    stack->type_size = type_size;
    stack->next_page_offset = (type_size * REPAN_STACK_GROWTH + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    stack->last_offset = 0;
}

void REPAN_PRIV(stack_free)(repan_stack *stack)
{
    uint8_t *current = stack->first;
    uint32_t next_page_offset = stack->next_page_offset;

    while (current != NULL) {
        uint8_t *next = *(uint8_t**)(current + next_page_offset);

        free(current);
        current = next;
    }

    if (stack->free_block) {
        free(stack->free_block);
    }
}

int REPAN_PRIV(stack_push)(repan_stack *stack)
{
    uint32_t type_size = stack->type_size;

    if (stack->last_offset == 0) {
        uint8_t *page;

        if (stack->free_block != NULL) {
            page = stack->free_block;
            stack->free_block = NULL;
        }
        else {
            page = (uint8_t*)malloc(stack->next_page_offset + sizeof(void*));
            if (page == NULL)
                return REPAN_FALSE;
        }

        *(uint8_t**)(page + stack->next_page_offset) = stack->first;
        stack->first = page;

        stack->last_offset = type_size * REPAN_STACK_GROWTH;
    }
    stack->last_offset -= type_size;

    return REPAN_TRUE;
}

void REPAN_PRIV(stack_pop)(repan_stack *stack)
{
    uint8_t *first;
    uint32_t type_size = stack->type_size;

    REPAN_ASSERT(!REPAN_STACK_IS_EMPTY(stack));

    stack->last_offset += type_size;

    if (stack->last_offset < type_size * REPAN_STACK_GROWTH)
        return;

    first = stack->first;

    if (stack->free_block == NULL) {
        stack->free_block = first;
    }

    stack->first = *(uint8_t**)(first + stack->next_page_offset);

    stack->last_offset = 0;
}
