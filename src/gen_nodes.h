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

#ifndef REPAN_GEN_NODES_H
#define REPAN_GEN_NODES_H

#define REPAN_GEN_STAR (1 << 8)
#define REPAN_GEN_PLUS (2 << 8)
#define REPAN_GEN_QUESTION_MARK (3 << 8)

#define REPAN_GET_CLASS_CHILDS(ptr) \
    ((ptr) == NULL ? NULL : &((repan_char_class_node*)(ptr))->node_list)
/* Useful if a single alternative is required. */
#define REPAN_GET_BRACKET_CHILDS(ptr) \
    ((ptr) == NULL ? NULL : (repan_prev_node*)(&((repan_bracket_node*)(ptr))->alt_node_list))
#define REPAN_GET_ALTERNATIVES(ptr) \
    ((ptr) == NULL ? NULL : &((repan_bracket_node*)(ptr))->alt_node_list)

repan_prev_node *REPAN_PRIV(gen_node)(repan_pattern *pattern,
    repan_prev_node *prev_node, uint8_t type, uint8_t sub_type);
repan_prev_node *REPAN_PRIV(gen_char)(repan_pattern *pattern,
    repan_prev_node *prev_node, uint8_t caseless, uint32_t chr);
repan_prev_node *REPAN_PRIV(gen_class)(repan_pattern *pattern,
    repan_prev_node *prev_node, uint8_t sub_type, uint8_t caseless);
repan_prev_node *REPAN_PRIV(gen_bracket)(repan_pattern *pattern,
    repan_prev_node *prev_node, uint8_t sub_type, uint32_t repeat);
repan_alt_node *REPAN_PRIV(gen_alternative)(repan_pattern *pattern,
    repan_alt_node *prev_alt_node);

#endif /* REPAN_GEN_NODES_H */
