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

#ifndef REPAN_NODES_H
#define REPAN_NODES_H

enum {
    /* Iterable node types. */
    REPAN_CHAR_NODE,
    REPAN_CHAR_RANGE_NODE,
    REPAN_PERL_CLASS_NODE,
    REPAN_U_PROPERTY_NODE,
    REPAN_POSIX_CLASS_NODE,

    /* Marks the end of node types allowed in a character class. */
    REPAN_CHAR_CLASS_NODE,

    REPAN_DOT_NODE,
    REPAN_NEWLINE_NODE,
    REPAN_GRAPHEME_CLUSTER_NODE,
    REPAN_CODE_UNIT_NODE,

    /* The NAMED variant must follow the next node. */
    REPAN_BACK_REFERENCE_NODE,
    REPAN_NAMED_BACK_REFERENCE_NODE,
    /* The NAMED variant must follow the next node. */
    REPAN_RECURSE_NODE,
    REPAN_NAMED_RECURSE_NODE,

    REPAN_BRACKET_NODE,
    REPAN_NODE_ITERABLE_MAX = REPAN_BRACKET_NODE,

    /* Non-iterable node types. */
    REPAN_ASSERT_CIRCUMFLEX_NODE,
    REPAN_ASSERT_DOLLAR_NODE,
    REPAN_ASSERT_WORD_BOUNDARY_NODE,
    REPAN_ASSERT_NOT_WORD_BOUNDARY_NODE,
    REPAN_ASSERT_SUBJECT_START_NODE,
    REPAN_ASSERT_MATCH_START_NODE,
    REPAN_ASSERT_SUBJECT_END_NODE,
    REPAN_ASSERT_SUBJECT_END_NEWLINE_NODE,

    REPAN_VERB_NODE,
    REPAN_VERB_WITH_ARGUMENT_NODE,
    REPAN_SET_MATCH_START_NODE,

    REPAN_LAST_NODE_INDEX
};

enum {
    REPAN_NON_CAPTURING_BRACKET,
    REPAN_CAPTURING_BRACKET,
    REPAN_NAMED_CAPTURING_BRACKET,
    REPAN_ATOMIC_BRACKET,
    REPAN_CAPTURE_RESET_BRACKET,
    REPAN_POSITIVE_LOOKAHEAD_BRACKET,
    REPAN_NEGATIVE_LOOKAHEAD_BRACKET,
    REPAN_POSITIVE_LOOKBEHIND_BRACKET,
    REPAN_NEGATIVE_LOOKBEHIND_BRACKET,
    REPAN_SCRIPT_RUN_BRACKET,

    /* The rest must be conditional blocks. */
    REPAN_COND_BRACKET_START,
    REPAN_COND_REFERENCE_BRACKET = REPAN_COND_BRACKET_START,
    REPAN_COND_NAMED_REFERENCE_BRACKET,
    REPAN_COND_RECURSE_BRACKET,
    REPAN_COND_NAMED_RECURSE_BRACKET,
    REPAN_COND_ASSERT_BRACKET,
    REPAN_COND_DEFINE_BRACKET,

    /* Special constant used by repan_opt_merge_alternatives(). */
    REPAN_MERGE_ALTERNATIVES_BRACKET,
};

enum {
    REPAN_BRACKET_IT_GREEDY,
    REPAN_BRACKET_IT_NON_GREEDY,
    REPAN_BRACKET_IT_POSSESSIVE,
};

enum {
    REPAN_NORMAL_CLASS,
    REPAN_NEGATED_CLASS,
};

#define REPAN_NODE_HEADER \
    struct repan_node_struct *next_node; \
    uint8_t type; \
    uint8_t sub_type;

typedef struct repan_node_struct {
    REPAN_NODE_HEADER;
} repan_node;

/* This type is compatible with both repan_node and repan_alt_node. */
typedef struct {
    repan_node *next_node;
} repan_prev_node;

typedef struct repan_alt_node_struct {
    repan_node *next_node;
    struct repan_alt_node_struct *next_alt_node;
} repan_alt_node;

typedef struct {
    REPAN_NODE_HEADER;
    uint8_t it_type;
    uint32_t it_min;
    uint32_t it_max;

    repan_alt_node alt_node_list;
} repan_bracket_node;

typedef struct {
    repan_bracket_node bracket_node;
    union {
        uint32_t number;
        repan_string *name;
    } u;
} repan_ext_bracket_node;

typedef struct {
    REPAN_NODE_HEADER;
    uint8_t caseless;
    uint32_t chr;
} repan_char_node;

typedef struct {
    REPAN_NODE_HEADER;
    uint8_t caseless;
    uint32_t chrs[2];
} repan_char_range_node;

typedef struct {
    REPAN_NODE_HEADER;
    uint8_t caseless;
    repan_prev_node node_list;
} repan_char_class_node;

typedef struct {
    REPAN_NODE_HEADER;
    union {
        uint32_t number;
        repan_string *name;
    } u;
} repan_reference_node;

typedef struct {
    REPAN_NODE_HEADER;
    uint8_t arg_separator;
    union {
        uint32_t number;
        repan_string *name;
    } u;
} repan_ext_verb_node;

typedef struct {
    REPAN_NODE_HEADER;
    uint16_t modifiers;
} repan_modifiers_node;

typedef struct {
    void *next;
    union {
        int count;
        void *dummy;
    } u;
} repan_block;

#define REPAN_RESERVED_BLOCK_COUNT 2

struct repan_pattern_struct {
    repan_bracket_node *bracket_node;
    repan_string *bracket_names;
    repan_string *verb_arguments;
    uint32_t options;
    uint32_t capture_count;

    repan_block *blocks[REPAN_RESERVED_BLOCK_COUNT];
};

/* To string only modifiers. */
#define REPAN_MODIFIER_HIDE_BRACKET REPAN_PARSE_PCRE_NO_AUTOCAPTURE

/* Pattern option flags. */
#define REPAN_CORE_OPTIONS (REPAN_PARSE_UTF | REPAN_PARSE_CASELESS | REPAN_PARSE_MULTILINE | REPAN_PARSE_DOT_ANY)
#define REPAN_PATTERN_DAMAGED 0x00000100

extern const uint8_t REPAN_PRIV(perl_class_list)[];

#endif /* REPAN_NODES_H */
