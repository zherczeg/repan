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
#include "parser.h"

uint32_t REPAN_PRIV(parse_decimal)(uint32_t **pattern_ref)
{
    uint32_t *pattern = *pattern_ref;
    uint32_t decimal = 0;
    uint32_t digit = *pattern++;

    do {
        if (decimal >= 100000000) {
            return REPAN_DECIMAL_INF;
        }

        decimal = decimal * 10 + digit - REPAN_CHAR_0;
        digit = *pattern++;
    } while (REPAN_IS_DECIMAL_DIGIT(digit));

    *pattern_ref = pattern - 1;
    return decimal;
}

uint32_t REPAN_PRIV(parse_posix_class)(uint32_t **pattern_start)
{
    uint32_t *pattern = *pattern_start;
    uint32_t *start;
    uint32_t data;
    int negated = REPAN_FALSE;

    REPAN_ASSERT(pattern[-1] == REPAN_CHAR_LEFT_SQUARE_BRACKET && pattern[0] == REPAN_CHAR_COLON);

    pattern++;

    if (*pattern == REPAN_CHAR_CIRCUMFLEX_ACCENT) {
        negated = REPAN_TRUE;
        pattern++;
    }

    start = pattern;

    while (REPAN_IS_LOWERCASE_LATIN(*pattern)) {
        pattern++;
    }

    if (pattern[0] != REPAN_CHAR_COLON || pattern[1] != REPAN_CHAR_RIGHT_SQUARE_BRACKET) {
        return 0;
    }

    if (pattern == start) {
        return 0;
    }

    data = REPAN_PRIV(find_posix_class)(start, pattern - start);

    if (data == 0) {
        return 0;
    }

    *pattern_start = pattern + 2;

    if (negated) {
        data += (data & REPAN_POSIX_OPT_IS_PERL) ? REPAN_NEG_PERL_CLASS : REPAN_NEG_POSIX_CLASS;
    }
    return data;
}

int REPAN_PRIV(parse_repeat)(repan_parser_context *context,
    repan_node **last_node_ref, repan_prev_node **prev_node_ref)
{
    repan_bracket_node *bracket_node;
    uint32_t *pattern = context->pattern;
    repan_node *last_node = *last_node_ref;
    repan_prev_node *prev_node = *prev_node_ref;
    uint8_t it_type = REPAN_BRACKET_IT_GREEDY;
    uint32_t it_min, it_max;
    uint32_t current_char = *pattern;

    REPAN_ASSERT(last_node->next_node == NULL);

    if (current_char != REPAN_CHAR_LEFT_BRACE) {
        if (current_char == REPAN_CHAR_ASTERISK) {
            it_min = 0;
            it_max = REPAN_DECIMAL_INF;
        }
        else if (current_char == REPAN_CHAR_PLUS) {
            it_min = 1;
            it_max = REPAN_DECIMAL_INF;
        }
        else {
            REPAN_ASSERT(current_char == REPAN_CHAR_QUESTION_MARK);
            it_min = 0;
            it_max = 1;
        }

        pattern++;
    }
    else {
        pattern++;

        if (!REPAN_IS_DECIMAL_DIGIT(*pattern)) {
            return REPAN_FALSE;
        }

        it_min = REPAN_PRIV(parse_decimal)(&pattern);
        if (it_min == REPAN_DECIMAL_INF) {
            context->error = REPAN_ERR_TOO_BIG_QUANTIFIER;
            context->pattern = pattern;
            return REPAN_FALSE;
        }

        it_max = it_min;
        if (*pattern == REPAN_CHAR_COMMA) {
            pattern++;
            it_max = REPAN_DECIMAL_INF;

            if (REPAN_IS_DECIMAL_DIGIT(*pattern)) {
                it_max = REPAN_PRIV(parse_decimal)(&pattern);

                if (it_max == REPAN_DECIMAL_INF) {
                    context->error = REPAN_ERR_TOO_BIG_QUANTIFIER;
                    context->pattern = pattern;
                    return REPAN_FALSE;
                }
            }
        }

        if (*pattern != REPAN_CHAR_RIGHT_BRACE) {
            return REPAN_FALSE;
        }

        if (it_max < it_min) {
            context->error = REPAN_ERR_QUANTIFIER_OUT_OF_ORDER;
            return REPAN_FALSE;
        }

        pattern++;
    }

    REPAN_ASSERT(last_node->type != REPAN_BRACKET_NODE
            || prev_node == NULL
            || prev_node == (repan_prev_node*)last_node);
    REPAN_ASSERT(last_node->type == REPAN_BRACKET_NODE
            || prev_node == NULL
            || prev_node->next_node == last_node);

    if (prev_node == NULL || last_node->type > REPAN_NODE_ITERABLE_MAX) {
        context->error = REPAN_ERR_NOTHING_TO_REPEAT;
        return REPAN_FALSE;
    }

    current_char = *pattern;
    if (current_char == REPAN_CHAR_QUESTION_MARK) {
        it_type = REPAN_BRACKET_IT_NON_GREEDY;
        pattern++;
    }
    else if (current_char == REPAN_CHAR_PLUS) {
        it_type = REPAN_BRACKET_IT_POSSESSIVE;
        pattern++;
    }

    context->pattern = pattern;

    if (it_min == it_max) {
        it_type = REPAN_BRACKET_IT_GREEDY;

        if (it_min == 1) {
            *prev_node_ref = NULL;
            return REPAN_TRUE;
        }
    }

    if (last_node->type != REPAN_BRACKET_NODE) {
        bracket_node = REPAN_ALLOC(repan_bracket_node, context->result);

        if (bracket_node == NULL) {
            context->error = REPAN_ERR_NO_MEMORY;
            return REPAN_FALSE;
        }

        bracket_node->next_node = NULL;
        bracket_node->type = REPAN_BRACKET_NODE;
        bracket_node->sub_type = REPAN_NON_CAPTURING_BRACKET;

        bracket_node->alt_node_list.next_node = last_node;
        bracket_node->alt_node_list.next_alt_node = NULL;

        prev_node->next_node = (repan_node*)bracket_node;
        *last_node_ref = (repan_node*)bracket_node;
    }
    else {
        bracket_node = (repan_bracket_node*)last_node;
    }

    bracket_node->it_type = it_type;
    bracket_node->it_min = it_min;
    bracket_node->it_max = it_max;
    *prev_node_ref = NULL;

    return REPAN_TRUE;
}

uint32_t REPAN_PRIV(u_parse_name)(repan_parser_context *context)
{
    uint8_t name[REPAN_U_MAX_NAME_LENGTH];
    uint32_t *pattern = context->pattern;
    uint32_t *pattern_end = context->pattern_end;
    uint8_t *dst, *end;
    uint32_t current_char;

    REPAN_ASSERT(pattern[-2] == REPAN_CHAR_N && pattern[-1] == REPAN_CHAR_LEFT_BRACE);

    if (!(context->options & REPAN_PARSE_UTF)) {
        context->error = REPAN_ERR_UTF_REQUIRED;
        context->pattern = pattern - 3;
        return 0;
    }

    dst = name;
    end = name + REPAN_U_MAX_NAME_LENGTH - 1;

    while (REPAN_TRUE) {
        if (dst >= end) {
            context->error = REPAN_ERR_INVALID_UNICODE_CODEPOINT_NAME;
            return 0;
        }

        if (pattern >= pattern_end) {
            context->error = REPAN_ERR_RIGHT_BRACE_EXPECTED;
            context->pattern = pattern;
            return 0;
        }

        current_char = *pattern;
        if (current_char == REPAN_CHAR_MINUS || current_char == REPAN_CHAR_SPACE) {
            int hypen_found = REPAN_FALSE;

            do {
                if (*pattern == REPAN_CHAR_MINUS) {
                    if (!hypen_found) {
                        *dst++ = REPAN_CHAR_EQUALS_SIGN;
                        hypen_found = REPAN_TRUE;
                    }
                }
                else if (*pattern != REPAN_CHAR_SPACE) {
                    break;
                }
                pattern++;
            } while (pattern < pattern_end);

            continue;
        }

        pattern++;

        if (REPAN_IS_DECIMAL_DIGIT(current_char)) {
            *dst++ = (uint8_t)current_char;
        }
        else if (REPAN_IS_CASELESS_LATIN(current_char)) {
            if (REPAN_IS_LOWERCASE_LATIN(current_char)) {
                current_char = REPAN_TO_UPPERCASE_LATIN(current_char);
            }

            *dst++ = (uint8_t)current_char;
        }
        else if (current_char == REPAN_CHAR_RIGHT_BRACE) {
            break;
        }
        else {
            context->error = REPAN_ERR_RIGHT_BRACE_EXPECTED;
            context->pattern = pattern - 1;
            return 0;
        }
    }

    if (dst == name) {
        context->error = REPAN_ERR_INVALID_UNICODE_CODEPOINT_NAME;
        return 0;
    }

    *dst = REPAN_CHAR_NUL;

    current_char = REPAN_PRIV(u_find_name)(name);

    if (current_char == UINT32_MAX) {
        context->error = REPAN_ERR_INVALID_UNICODE_CODEPOINT_NAME;
        return 0;
    }

    context->pattern = pattern;
    return current_char;
}

static int u_is_string_equal(uint32_t *str1, const char *str2)
{
    do {
        if (*str1++ != *str2++) {
            return REPAN_FALSE;
        }
    } while (*str2 != REPAN_CHAR_NUL);

    return REPAN_TRUE;
}

enum {
    REPAN_U_PARSE_PROPERTY_MODE_NOT_SET,
    REPAN_U_PARSE_PROPERTY_MODE_SCRIPT,
    REPAN_U_PARSE_PROPERTY_MODE_SCRIPT_EXTENSION,
    REPAN_U_PARSE_PROPERTY_MODE_GENERAL_CATHEGORY,
};

static int u_get_mode_strict(uint32_t *start, uint32_t *end)
{
    switch (end - start) {
    case 2:
        if (u_is_string_equal(start, "sc")) {
            return REPAN_U_PARSE_PROPERTY_MODE_SCRIPT;
        }
        if (u_is_string_equal(start, "gc")) {
            return REPAN_U_PARSE_PROPERTY_MODE_GENERAL_CATHEGORY;
        }
        break;
    case 3:
        if (u_is_string_equal(start, "scx")) {
            return REPAN_U_PARSE_PROPERTY_MODE_SCRIPT_EXTENSION;
        }
        break;
    case 6:
        if (u_is_string_equal(start, "Script")) {
            return REPAN_U_PARSE_PROPERTY_MODE_SCRIPT;
        }
        break;
    case 16:
        if (u_is_string_equal(start, "General_Category")) {
            return REPAN_U_PARSE_PROPERTY_MODE_GENERAL_CATHEGORY;
        }
        break;
    case 17:
        if (u_is_string_equal(start, "Script_Extensions")) {
            return REPAN_U_PARSE_PROPERTY_MODE_SCRIPT_EXTENSION;
        }
        break;
    }

    return REPAN_U_PARSE_PROPERTY_MODE_NOT_SET;
}

static int u_get_mode_non_strict(uint32_t *start, uint32_t *end)
{
    switch (end - start) {
    case 2:
        if (u_is_string_equal(start, "SC")) {
            return REPAN_U_PARSE_PROPERTY_MODE_SCRIPT;
        }
        if (u_is_string_equal(start, "GC")) {
            return REPAN_U_PARSE_PROPERTY_MODE_GENERAL_CATHEGORY;
        }
        break;
    case 3:
        if (u_is_string_equal(start, "SCX")) {
            return REPAN_U_PARSE_PROPERTY_MODE_SCRIPT_EXTENSION;
        }
        break;
    case 6:
        if (u_is_string_equal(start, "SCRIPT")) {
            return REPAN_U_PARSE_PROPERTY_MODE_SCRIPT;
        }
        break;
    case 15:
        if (u_is_string_equal(start, "GENERALCATEGORY")) {
            return REPAN_U_PARSE_PROPERTY_MODE_GENERAL_CATHEGORY;
        }
        break;
    case 16:
        if (u_is_string_equal(start, "SCRIPTEXTENSIONS")) {
            return REPAN_U_PARSE_PROPERTY_MODE_SCRIPT_EXTENSION;
        }
        break;
    }

    return REPAN_U_PARSE_PROPERTY_MODE_NOT_SET;
}

repan_node *REPAN_PRIV(u_parse_property)(repan_parser_context *context, int is_strict)
{
    uint32_t name[REPAN_U_MAX_NAME_LENGTH];
    repan_u_property_node *property_node;
    uint32_t *pattern = context->pattern;
    uint32_t *start = context->pattern - 2;
    uint32_t *dst = name;
    uint32_t *end = name + REPAN_U_MAX_NAME_LENGTH - 1;
    uint32_t data, prop_info;
    uint8_t sub_type = 0;
    int mode = REPAN_U_PARSE_PROPERTY_MODE_NOT_SET;

    REPAN_ASSERT(pattern[-1] == REPAN_CHAR_p || pattern[-1] == REPAN_CHAR_P);

    if (!(context->options & REPAN_PARSE_UTF)) {
        context->error = REPAN_ERR_UTF_REQUIRED;
        context->pattern = pattern - 2;
        return NULL;
    }

    if (pattern[-1] == REPAN_CHAR_P) {
        sub_type = REPAN_U_NEGATED_PROPERTY;
    }

    if (*pattern != REPAN_CHAR_LEFT_BRACE) {
        uint32_t chr = *pattern++;

        if (!REPAN_IS_CASELESS_LATIN(chr)) {
            context->error = REPAN_ERR_INVALID_P_SEQUENCE;
            context->pattern = start;
            return NULL;
        }

        *dst++ = chr;
        end = pattern;
    }
    else {
        pattern++;
        context->pattern = pattern;

        while (REPAN_TRUE) {
            uint32_t chr = *pattern;

            if (chr == REPAN_CHAR_SPACE || chr == REPAN_CHAR_UNDERSCORE || chr == REPAN_CHAR_MINUS) {
                pattern++;
                continue;
            }

            if (chr == REPAN_CHAR_RIGHT_BRACE) {
                break;
            }

            if (chr == REPAN_CHAR_EQUALS_SIGN) {
                if (mode != REPAN_U_PARSE_PROPERTY_MODE_NOT_SET) {
                    context->error = REPAN_ERR_INVALID_P_SEQUENCE;
                    context->pattern = start;
                    return NULL;
                }

                if (is_strict) {
                    mode = u_get_mode_strict(context->pattern, pattern);
                }
                else {
                    mode = u_get_mode_non_strict(name, dst);
                }

                if (mode == REPAN_U_PARSE_PROPERTY_MODE_NOT_SET) {
                    context->error = REPAN_ERR_INVALID_P_SEQUENCE;
                    context->pattern = start;
                    return NULL;
                }

                dst = name;
                pattern++;
                context->pattern = pattern;
                continue;
            }

            if (REPAN_IS_CASELESS_LATIN(chr) || chr == REPAN_CHAR_AMPERSAND) {
                if (REPAN_IS_LOWERCASE_LATIN(chr)) {
                    chr = REPAN_TO_UPPERCASE_LATIN(chr);
                }
                *dst++ = chr;
                if (dst <= end) {
                    pattern++;
                    continue;
                }
            }

            if (pattern >= context->pattern_end) {
                context->error = REPAN_ERR_RIGHT_BRACE_EXPECTED;
                context->pattern = pattern;
                return NULL;
            }

            context->error = REPAN_ERR_INVALID_P_SEQUENCE;
            context->pattern = start;
            return NULL;
        }

        end = pattern;
        pattern++;
    }

    if (is_strict) {
        uint32_t prop_index = REPAN_PRIV(find_u_property)(name, dst - name, REPAN_TRUE);

        data = 0;
        if (prop_index != 0) {
            prop_index--;
            const char *string = REPAN_PRIV(u_property_names)[prop_index].string;

            if (strlen(string) != (size_t)(end - context->pattern) || !u_is_string_equal(context->pattern, string)) {
                context->error = REPAN_ERR_UNKNOWN_PROPERTY;

                if (mode == REPAN_U_PARSE_PROPERTY_MODE_NOT_SET) {
                    context->pattern = start;
                }
                return NULL;
            }

            data = REPAN_PRIV(u_properties)[prop_index].data;
        }
    }
    else {
        data = REPAN_PRIV(find_u_property)(name, dst - name, REPAN_FALSE);
    }

    if (data == 0) {
        context->error = REPAN_ERR_UNKNOWN_PROPERTY;

        if (mode == REPAN_U_PARSE_PROPERTY_MODE_NOT_SET) {
            context->pattern = start;
        }
        return NULL;
    }

    switch (mode) {
    case REPAN_U_PARSE_PROPERTY_MODE_SCRIPT_EXTENSION:
        sub_type = (uint8_t)(sub_type | REPAN_U_SCRIPT_EXTENSION);
        /* Fallthrough. */
    case REPAN_U_PARSE_PROPERTY_MODE_SCRIPT:
        prop_info = REPAN_PRIV(u_property_list)[REPAN_U_GET_PROPERTY(data)];

        if ((prop_info & REPAN_U_DEFINE_SCRIPT) != REPAN_U_DEFINE_SCRIPT) {
            context->error = REPAN_ERR_UNICODE_SCRIPT_EXPECTED;
            return NULL;
        }
        break;
    case REPAN_U_PARSE_PROPERTY_MODE_GENERAL_CATHEGORY:
        prop_info = REPAN_PRIV(u_property_list)[REPAN_U_GET_PROPERTY(data)];

        if (!(prop_info & REPAN_U_IS_CATHEGORY)) {
            context->error = REPAN_ERR_UNICODE_CATHEGORY_EXPECTED;
            return NULL;
        }
        break;
    }

    property_node = REPAN_ALLOC(repan_u_property_node, context->result);
    property_node->next_node = NULL;
    property_node->type = REPAN_U_PROPERTY_NODE;
    property_node->sub_type = sub_type;
    property_node->property = data & REPAN_U_PROPERTY_TYPE_MASK;

    context->pattern = pattern;
    return (repan_node*)property_node;
}
