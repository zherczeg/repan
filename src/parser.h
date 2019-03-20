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

#ifndef REPAN_PARSER_H
#define REPAN_PARSER_H

typedef struct repan_undefined_name_struct {
    struct repan_undefined_name_struct *next;
    uint32_t *start;
    repan_string *name;
} repan_undefined_name;

/* Context for parsing. */
typedef struct {
    uint32_t error;

    /* Zero terminated string, even though the
       pattern may contain zero character. */
    uint32_t *pattern;
    uint32_t *pattern_start;
    uint32_t *pattern_end;
    uint32_t char_max;

    uint32_t options;
    repan_parse_extra_opts *extra_opts;

    repan_stack stack;
    repan_undefined_name *undefined_names;
    repan_pattern *result;
} repan_parser_context;

void REPAN_PRIV(parse_pcre_bracket)(repan_parser_context *context);
void REPAN_PRIV(parse_javascript_bracket)(repan_parser_context *context);
void REPAN_PRIV(parse_posix_bracket)(repan_parser_context *context);
void REPAN_PRIV(parse_glob)(repan_parser_context *context);

uint32_t REPAN_PRIV(parse_decimal)(uint32_t **pattern_ref);
uint32_t REPAN_PRIV(parse_posix_class)(uint32_t **pattern_start);
int REPAN_PRIV(parse_repeat)(repan_parser_context *context,
    repan_node **last_node_ref, repan_prev_node **prev_node_ref);
uint32_t REPAN_PRIV(u_parse_name)(repan_parser_context *context);
repan_node *REPAN_PRIV(u_parse_property)(repan_parser_context *context, int is_strict);

#endif /* REPAN_PARSER_H */
