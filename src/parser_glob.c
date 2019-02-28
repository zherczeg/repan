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

/* Converting any input to uint32_t characters. */

#include "internal.h"
#include "parser.h"

#define REPAN_POSIX_DIR_SEPARATOR '/'

typedef struct {
    repan_bracket_node *ungreedy_target;
    uint8_t commit_allowed;
} repan_parser_locals;

static repan_prev_node *create_node(repan_parser_context *context, repan_prev_node *prev_node, uint8_t type)
{
    repan_node *node = REPAN_ALLOC(repan_node, context->result);

    if (node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        return NULL;
    }

    prev_node->next_node = node;
    node->next_node = NULL;
    node->type = type;

    return (repan_prev_node*)node;
}

static repan_bracket_node *create_bracket_node(repan_parser_context *context)
{
    repan_bracket_node *bracket_node = REPAN_ALLOC(repan_bracket_node, context->result);

    if (bracket_node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        return NULL;
    }

    bracket_node->next_node = NULL;
    bracket_node->type = REPAN_BRACKET_NODE;
    bracket_node->sub_type = REPAN_NON_CAPTURING_BRACKET;
    bracket_node->it_type = REPAN_BRACKET_IT_GREEDY;
    bracket_node->it_min = 1;
    bracket_node->it_max = 1;
    bracket_node->alt_node_list.next_node = NULL;
    bracket_node->alt_node_list.next_alt_node = NULL;
    return bracket_node;
}

static repan_prev_node *create_char_node(repan_parser_context *context,
    repan_prev_node *prev_node, uint32_t chr)
{
    repan_char_node *char_node = REPAN_ALLOC(repan_char_node, context->result);

    if (char_node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        return NULL;
    }

    char_node->next_node = NULL;
    char_node->type = REPAN_CHAR_NODE;
    char_node->caseless = REPAN_FALSE;
    char_node->chr = chr;

    prev_node->next_node = (repan_node*)char_node;
    return (repan_prev_node*)char_node;
}

static repan_prev_node *create_neg_class_node(repan_parser_context *context,
    repan_prev_node *prev_node, uint32_t chr1, uint32_t chr2)
{
    repan_char_class_node *char_class_node = REPAN_ALLOC(repan_char_class_node, context->result);

    if (char_class_node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        return NULL;
    }

    char_class_node->next_node = NULL;
    char_class_node->type = REPAN_CHAR_CLASS_NODE;
    char_class_node->sub_type = REPAN_NEGATED_CLASS;
    char_class_node->caseless = REPAN_FALSE;
    char_class_node->node_list.next_node = NULL;

    prev_node->next_node = (repan_node*)char_class_node;

    prev_node = create_char_node(context, &char_class_node->node_list, chr1);

    if (prev_node == NULL) {
        return NULL;
    }

    if (chr2 != REPAN_CHAR_NUL) {
        if (create_char_node(context, prev_node, chr2) == NULL) {
            return NULL;
        }
    }

    return (repan_prev_node*)char_class_node;
}

static repan_prev_node *create_bracketed_neg_class_node(repan_parser_context *context,
    repan_prev_node *prev_node, uint32_t chr1, uint32_t chr2)
{
    repan_bracket_node *bracket_node = create_bracket_node(context);

    if (bracket_node == NULL) {
        return NULL;
    }

    prev_node->next_node = (repan_node*)bracket_node;

    if (create_neg_class_node(context, (repan_prev_node*)&bracket_node->alt_node_list, chr1, chr2) == NULL) {
        return NULL;
    }

    return (repan_prev_node*)bracket_node;
}

static repan_prev_node *create_posix_star_after_dir_separator(repan_parser_context *context,
    repan_parser_locals *locals, repan_prev_node *prev_node, int followed_by_dot)
{
    repan_bracket_node *result_bracket_node = NULL;
    repan_bracket_node *bracket_node = NULL;

    if (!followed_by_dot) {
        result_bracket_node = create_bracket_node(context);

        if (result_bracket_node == NULL) {
            return NULL;
        }

        result_bracket_node->it_min = 0;
        prev_node->next_node = (repan_node*)result_bracket_node;

        prev_node = (repan_prev_node*)&result_bracket_node->alt_node_list;
    }

    prev_node = create_neg_class_node(context, prev_node, REPAN_POSIX_DIR_SEPARATOR, REPAN_CHAR_DOT);
    if (prev_node == NULL) {
        return NULL;
    }

    prev_node = create_bracketed_neg_class_node(context, prev_node, REPAN_POSIX_DIR_SEPARATOR, REPAN_CHAR_NUL);

    if (prev_node == NULL) {
        return NULL;
    }

    bracket_node = (repan_bracket_node*)prev_node;
    bracket_node->it_min = 0;
    bracket_node->it_max = REPAN_DECIMAL_INF;

    if (context->pattern >= context->pattern_end || *context->pattern == REPAN_POSIX_DIR_SEPARATOR) {
        bracket_node->it_type = REPAN_BRACKET_IT_POSSESSIVE;
    }
    else {
        locals->ungreedy_target = bracket_node;
    }

    return followed_by_dot ? prev_node : (repan_prev_node*)result_bracket_node;
}

static repan_prev_node *create_posix_double_star(repan_parser_context *context, repan_prev_node *prev_node)
{
    repan_bracket_node *bracket_node = create_bracket_node(context);
    repan_bracket_node *sub_bracket_node;
    repan_alt_node *alt_node;

    if (bracket_node == NULL) {
        return NULL;
    }

    bracket_node->sub_type = REPAN_ATOMIC_BRACKET;
    bracket_node->it_min = 0;
    bracket_node->it_max = REPAN_DECIMAL_INF;
    prev_node->next_node = (repan_node*)bracket_node;

    prev_node = (repan_prev_node*)&bracket_node->alt_node_list;
    prev_node = create_neg_class_node(context, prev_node, REPAN_POSIX_DIR_SEPARATOR, REPAN_CHAR_DOT);

    if (prev_node == NULL) {
        return NULL;
    }

    prev_node = create_bracketed_neg_class_node(context, prev_node, REPAN_POSIX_DIR_SEPARATOR, REPAN_CHAR_NUL);

    if (prev_node == NULL) {
        return NULL;
    }

    sub_bracket_node = (repan_bracket_node*)prev_node;
    sub_bracket_node->it_type = REPAN_BRACKET_IT_POSSESSIVE;
    sub_bracket_node->it_min = 0;
    sub_bracket_node->it_max = REPAN_DECIMAL_INF;

    if (create_char_node(context, prev_node, REPAN_POSIX_DIR_SEPARATOR) == NULL) {
        return NULL;
    }

    alt_node = REPAN_ALLOC(repan_alt_node, context->result);

    if (alt_node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        return NULL;
    }

    alt_node->next_node = NULL;
    alt_node->next_alt_node = NULL;

    bracket_node->alt_node_list.next_alt_node = alt_node;

    prev_node = (repan_prev_node*)alt_node;

    sub_bracket_node = create_bracket_node(context);

    if (sub_bracket_node == NULL) {
        return NULL;
    }

    sub_bracket_node->it_type = REPAN_BRACKET_IT_POSSESSIVE;
    sub_bracket_node->it_min = 0;

    prev_node->next_node = (repan_node*)sub_bracket_node;
    prev_node = (repan_prev_node*)sub_bracket_node;

    if (create_char_node(context, (repan_prev_node*)&sub_bracket_node->alt_node_list, REPAN_CHAR_DOT) == NULL) {
        return NULL;
    }

    if (create_char_node(context, prev_node, REPAN_POSIX_DIR_SEPARATOR) == NULL) {
        return NULL;
    }

    return (repan_prev_node*)bracket_node;
}

static repan_prev_node *process_posix_wildchars(repan_parser_context *context,
    repan_parser_locals *locals, repan_prev_node *prev_node)
{
    uint32_t count;
    int is_repeat;
    int is_dir_separator = (context->pattern == context->pattern_start || context->pattern[-1] == REPAN_POSIX_DIR_SEPARATOR);
    repan_bracket_node *bracket_node;

    REPAN_ASSERT(*context->pattern == REPAN_CHAR_QUESTION_MARK || *context->pattern == REPAN_CHAR_ASTERISK);

    if (context->pattern[0] == REPAN_CHAR_ASTERISK && context->pattern[1] == REPAN_CHAR_ASTERISK) {
        if ((context->pattern > context->pattern_start && context->pattern[-1] != REPAN_POSIX_DIR_SEPARATOR)
                || (context->pattern + 2 < context->pattern_end && context->pattern[2] != REPAN_POSIX_DIR_SEPARATOR))
        {
            context->error = REPAN_ERR_GLOB_INVALID_ASTERISK;
            return NULL;
        }

        locals->commit_allowed = REPAN_FALSE;

        prev_node = create_posix_double_star(context, prev_node);
        if (prev_node == NULL) {
            return NULL;
        }

        context->pattern += 2;

        if (*context->pattern == REPAN_POSIX_DIR_SEPARATOR) {
            context->pattern++;
            return prev_node;
        }

        return create_posix_star_after_dir_separator(context, locals, prev_node, REPAN_TRUE);
    }

    is_repeat = REPAN_FALSE;
    count = 0;
    while (REPAN_TRUE) {
       if (*context->pattern == REPAN_CHAR_QUESTION_MARK) {
           count++;
           if (count > REPAN_RESOURCE_MAX) {
               context->error = REPAN_ERR_LENGTH_LIMIT;
               return NULL;
           }
       }
       else if (*context->pattern == REPAN_CHAR_ASTERISK) {
           if (context->pattern[1] == REPAN_CHAR_ASTERISK) {
               context->error = REPAN_ERR_GLOB_INVALID_ASTERISK;
               return NULL;
           }
           is_repeat = REPAN_TRUE;
       }
       else {
           break;
       }
       context->pattern++;
    }

    if (is_repeat && locals->commit_allowed && locals->ungreedy_target != NULL) {
        REPAN_ASSERT(locals->ungreedy_target->it_max == REPAN_DECIMAL_INF);
        locals->ungreedy_target->it_type = REPAN_BRACKET_IT_NON_GREEDY;
        locals->ungreedy_target = NULL;

        prev_node = create_node(context, prev_node, REPAN_VERB_NODE);
        if (prev_node == NULL) {
            return NULL;
        }

        ((repan_node*)prev_node)->sub_type = REPAN_KEYW_COMMIT;
    }

    if (is_dir_separator) {
        if (count == 0) {
            int followed_by_dot = (context->pattern == context->pattern_end)
                    || (*context->pattern == REPAN_CHAR_DOT);

            REPAN_ASSERT(is_repeat);
            return create_posix_star_after_dir_separator(context, locals, prev_node, followed_by_dot);
        }

        prev_node = create_neg_class_node(context, prev_node, REPAN_POSIX_DIR_SEPARATOR, REPAN_CHAR_DOT);
        if (prev_node == NULL) {
            return NULL;
        }

        --count;

        if (count == 0 && !is_repeat) {
            return prev_node;
        }
    }

    if (count == 1 && !is_repeat) {
        return create_neg_class_node(context, prev_node, REPAN_POSIX_DIR_SEPARATOR, REPAN_CHAR_NUL);
    }

    prev_node = create_bracketed_neg_class_node(context, prev_node, REPAN_POSIX_DIR_SEPARATOR, REPAN_CHAR_NUL);

    if (prev_node == NULL) {
        return NULL;
    }

    bracket_node = (repan_bracket_node*)prev_node;
    bracket_node->it_min = count;
    bracket_node->it_max = count;

    if (is_repeat) {
        bracket_node->it_max = REPAN_DECIMAL_INF;
        locals->ungreedy_target = bracket_node;

        if (context->pattern >= context->pattern_end || *context->pattern == REPAN_POSIX_DIR_SEPARATOR) {
            bracket_node->it_type = REPAN_BRACKET_IT_POSSESSIVE;
        }
    }

    return prev_node;
}

void REPAN_PRIV(parse_glob)(repan_parser_context *context)
{
    repan_parser_locals locals;
    repan_prev_node *prev_node;
    repan_bracket_node *bracket_node;
    uint32_t *pattern_end;

    /* Currently unused. */
    REPAN_PRIV(stack_init)(&context->stack, sizeof(size_t));

    bracket_node = create_bracket_node(context);

    if (bracket_node == NULL) {
        return;
    }

    context->result->bracket_node = bracket_node;
    prev_node = (repan_prev_node *)&bracket_node->alt_node_list;
    pattern_end = context->pattern_end;

    prev_node = create_node(context, prev_node, REPAN_ASSERT_SUBJECT_START_NODE);
    if (prev_node == NULL) {
        return;
    }

    locals.ungreedy_target = NULL;
    locals.commit_allowed = REPAN_TRUE;

    while (context->pattern < pattern_end) {
        uint32_t current_char = *context->pattern;

        switch (current_char) {
        case REPAN_CHAR_QUESTION_MARK:
        case REPAN_CHAR_ASTERISK:
            prev_node = process_posix_wildchars(context, &locals, prev_node);

            if (context->error != REPAN_SUCCESS) {
                return;
            }
            continue;
        }

        prev_node = create_char_node(context, prev_node, current_char);

        if (prev_node == NULL) {
            return;
        }

        context->pattern++;
    }

    create_node(context, prev_node, REPAN_ASSERT_SUBJECT_END_NODE);
}
