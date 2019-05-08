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

repan_prev_node *REPAN_PRIV(gen_node)(repan_pattern *pattern,
    repan_prev_node *prev_node, uint8_t type, uint8_t sub_type)
{
    repan_node *node;

    if (prev_node == NULL) {
        REPAN_ASSERT(pattern->options & REPAN_PATTERN_DAMAGED);
        return NULL;
    }

    node = REPAN_ALLOC(repan_node, pattern);

    if (node == NULL) {
        pattern->options |= REPAN_PATTERN_DAMAGED;
        return NULL;
    }

    node->next_node = prev_node->next_node;
    prev_node->next_node = node;

    node->type = type;
    node->sub_type = sub_type;

    return (repan_prev_node*)node;
}

repan_prev_node *REPAN_PRIV(gen_char)(repan_pattern *pattern,
    repan_prev_node *prev_node, uint8_t caseless, uint32_t chr)
{
    repan_char_node *char_node;

    if (prev_node == NULL) {
        REPAN_ASSERT(pattern->options & REPAN_PATTERN_DAMAGED);
        return NULL;
    }

    char_node = REPAN_ALLOC(repan_char_node, pattern);

    if (char_node == NULL) {
        pattern->options |= REPAN_PATTERN_DAMAGED;
        return NULL;
    }

    char_node->next_node = prev_node->next_node;
    prev_node->next_node = (repan_node*)char_node;

    char_node->type = REPAN_CHAR_NODE;
    char_node->caseless = caseless;
    char_node->chr = chr;

    return (repan_prev_node*)char_node;
}

repan_prev_node *REPAN_PRIV(gen_class)(repan_pattern *pattern,
    repan_prev_node *prev_node, uint8_t sub_type, uint8_t caseless)
{
    repan_char_class_node *char_class_node;

    if (prev_node == NULL) {
        REPAN_ASSERT(pattern->options & REPAN_PATTERN_DAMAGED);
        return NULL;
    }

    char_class_node = REPAN_ALLOC(repan_char_class_node, pattern);

    if (char_class_node == NULL) {
        pattern->options |= REPAN_PATTERN_DAMAGED;
        return NULL;
    }

    char_class_node->next_node = prev_node->next_node;
    prev_node->next_node = (repan_node*)char_class_node;

    char_class_node->type = REPAN_CHAR_CLASS_NODE;
    char_class_node->sub_type = sub_type;
    char_class_node->caseless = caseless;
    char_class_node->node_list.next_node = NULL;

    return (repan_prev_node*)char_class_node;
}

repan_prev_node *REPAN_PRIV(gen_bracket)(repan_pattern *pattern,
    repan_prev_node *prev_node, uint8_t sub_type, uint32_t repeat)
{
    repan_bracket_node *bracket_node;

    if (prev_node == NULL) {
        REPAN_ASSERT(pattern->options & REPAN_PATTERN_DAMAGED);
        return NULL;
    }

    bracket_node = REPAN_ALLOC(repan_bracket_node, pattern);

    if (bracket_node == NULL) {
        pattern->options |= REPAN_PATTERN_DAMAGED;
        return NULL;
    }

    bracket_node->next_node = prev_node->next_node;
    prev_node->next_node = (repan_node*)bracket_node;

    bracket_node->next_node = NULL;
    bracket_node->type = REPAN_BRACKET_NODE;
    bracket_node->sub_type = sub_type;
    bracket_node->it_type = (uint8_t)repeat;

    switch (repeat & ~0xff) {
    case REPAN_GEN_STAR:
        bracket_node->it_min = 0;
        bracket_node->it_max = REPAN_DECIMAL_INF;
        break;
    case REPAN_GEN_PLUS:
        bracket_node->it_min = 1;
        bracket_node->it_max = REPAN_DECIMAL_INF;
        break;
    case REPAN_GEN_QUESTION_MARK:
        bracket_node->it_min = 0;
        bracket_node->it_max = 1;
        break;
    default:
        bracket_node->it_min = 1;
        bracket_node->it_max = 1;
        break;
    }

    bracket_node->alt_node_list.next_node = NULL;
    bracket_node->alt_node_list.next_alt_node = NULL;

    return (repan_prev_node*)bracket_node;
}

repan_alt_node *REPAN_PRIV(gen_alternative)(repan_pattern *pattern,
    repan_alt_node *prev_alt_node)
{
    repan_alt_node *alt_node;

    if (prev_alt_node == NULL) {
        REPAN_ASSERT(pattern->options & REPAN_PATTERN_DAMAGED);
        return NULL;
    }

    alt_node = REPAN_ALLOC(repan_alt_node, pattern);

    if (alt_node == NULL) {
        pattern->options |= REPAN_PATTERN_DAMAGED;
        return NULL;
    }

    alt_node->next_alt_node = prev_alt_node->next_alt_node;
    prev_alt_node->next_alt_node = alt_node;

    alt_node->next_node = NULL;
    return alt_node;
}
