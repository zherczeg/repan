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

#ifndef REPAN_ALLOC_H
#define REPAN_ALLOC_H

#define REPAN_ALLOC(type, pattern) \
    ((type*)REPAN_PRIV(alloc)((pattern), sizeof(type)))

void *REPAN_PRIV(alloc)(repan_pattern *pattern, size_t block_size);
void REPAN_PRIV(free)(repan_pattern *pattern, void *block, size_t block_size);

#define REPAN_STRING_LENGTH_SHIFT 8
#define REPAN_STRING_DEFINED 0x1
#define REPAN_STRING_REFERENCED 0x2

typedef struct repan_string_struct {
    struct repan_string_struct *next;
    uint32_t length_and_flags;
    uint32_t index;
    uint32_t string[1];
} repan_string;

repan_string *REPAN_PRIV(string_add)(repan_string **strings, uint32_t *start, size_t length, uint32_t *error);

typedef struct {
    uint8_t *first;
    uint8_t *free_block;

    uint32_t type_size;
    uint32_t next_page_offset;
    uint32_t last_offset;
} repan_stack;

#define REPAN_STACK_IS_EMPTY(stack) \
    ((stack)->first == NULL)

#define REPAN_STACK_TOP(type, stack) \
    (*(type*)((stack)->first + (stack)->last_offset))

#define REPAN_STACK_TOP_PTR(type, stack) \
    ((type*)((stack)->first + (stack)->last_offset))

void REPAN_PRIV(stack_init)(repan_stack *stack, size_t type_size);
void REPAN_PRIV(stack_free)(repan_stack *stack);

int REPAN_PRIV(stack_push)(repan_stack *stack);
void REPAN_PRIV(stack_pop)(repan_stack *stack);

#endif /* REPAN_ALLOC_H */
