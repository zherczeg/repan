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

/* This file is inlined by parser_pcre.c. */

static uint32_t parse_hex(repan_parser_context *context, int expected_chars)
{
    uint32_t value = 0;
    uint32_t *pattern = context->pattern;
    int remaining_chars = expected_chars;

    do {
        uint32_t current_char = *pattern;

        if (REPAN_IS_DECIMAL_DIGIT(current_char)) {
            value = (value << 4) | (current_char - REPAN_CHAR_0);
        }
        else if (current_char >= REPAN_CHAR_A && current_char <= REPAN_CHAR_F) {
            value = (value << 4) | (current_char - (REPAN_CHAR_A - 10));
        }
        else if (current_char >= REPAN_CHAR_a && current_char <= REPAN_CHAR_f) {
            value = (value << 4) | (current_char - (REPAN_CHAR_a - 10));
        }
        else if (expected_chars > 4) {
            break;
        }
        else {
            return UINT32_MAX;
        }

        pattern++;
    } while (--remaining_chars > 0);

    context->pattern = pattern;
    return value;
}

static uint32_t parse_oct(repan_parser_context *context, int max_chars)
{
    uint32_t value = 0;

    do {
        uint32_t current_char = *context->pattern;

        if (current_char >= REPAN_CHAR_0 && current_char <= REPAN_CHAR_7) {
            value = (value << 3) | (current_char - REPAN_CHAR_0);
        }
        else {
            return value;
        }

        context->pattern++;
    } while (--max_chars > 0);

    return value;
}

static uint32_t parse_unicode_escape(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t result;
    uint32_t *start = context->pattern - 2;
    uint32_t *pattern;

    REPAN_ASSERT(context->pattern[-1] == REPAN_CHAR_u);

    if (*context->pattern == REPAN_CHAR_LEFT_BRACE) {
        context->pattern++;

        pattern = context->pattern;

        while (*context->pattern == REPAN_CHAR_0) {
            context->pattern++;
        }

        result = parse_hex(context, 8);

        if (pattern == context->pattern) {
            context->error = REPAN_ERR_HEXADECIMAL_NUMBER_REQUIRED;
            return 0;
        }

        if (*context->pattern != REPAN_CHAR_RIGHT_BRACE) {
            context->error = REPAN_ERR_RIGHT_BRACE_EXPECTED;
            context->pattern = start;
            return 0;
        }

        context->pattern++;
    }
    else {
        result = parse_hex(context, 4);

        if (result == UINT32_MAX) {
            return REPAN_CHAR_u;
        }

        if ((context->options & REPAN_PARSE_UTF) && (result >= 0xd800 && result < 0xdc00)
                && context->pattern[0] == REPAN_CHAR_BACKSLASH && context->pattern[1] == REPAN_CHAR_u) {
            uint32_t low_surrogate;

            start = context->pattern;
            context->pattern += 2;
            low_surrogate = parse_hex(context, 4);

            if (low_surrogate >= 0xdc00 && low_surrogate < 0xe000) {
                result = (((result & 0x3ff) << 10) | (low_surrogate & 0x3ff)) + 0x10000;
            }
            else {
                context->pattern = start;
                start -= 6;
            }
        }
    }

    if (result > context->char_max) {
        context->error = REPAN_ERR_TOO_LARGE_CHARACTER;
        context->pattern = start;
        return 0;
    }

    if ((context->options & REPAN_PARSE_UTF) && (result >= 0xd800 && result < 0xe000)) {
        context->error = REPAN_ERR_INVALID_UTF_CHAR;
        context->pattern = start;
        return 0;
    }

    return result;
}

static repan_string *parse_name(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t *pattern = context->pattern;
    uint32_t *start = context->pattern;
    uint32_t *string;
    uint32_t chr;
    repan_string *name;
    size_t length = 0;
    int first_char = REPAN_TRUE;

    REPAN_ASSERT(pattern[-1] == REPAN_CHAR_LESS_THAN_SIGN);

    while (REPAN_TRUE) {
        uint32_t *chr_start = pattern;

        if (*pattern == REPAN_CHAR_GREATER_THAN_SIGN) {
            break;
        }

        if (pattern[0] == REPAN_CHAR_BACKSLASH && pattern[1] == REPAN_CHAR_u) {
            pattern += 2;
            context->pattern = pattern;

            chr = parse_unicode_escape(context, locals);
            if (context->error != REPAN_SUCCESS) {
                return NULL;
            }
            if (context->pattern == pattern) {
                context->error = REPAN_ERR_INVALID_U_SEQUENCE;
                context->pattern -= 2;
                return NULL;
            }
            pattern = context->pattern;
        }
        else {
            chr = *pattern++;
        }

        if (first_char) {
            first_char = REPAN_FALSE;
            length++;

            if (context->options & REPAN_PARSE_UTF) {
                if (REPAN_PRIV(u_match_property)(chr, REPAN_UP_ID_START)) {
                    continue;
                }
            }
            else if (!REPAN_IS_DECIMAL_DIGIT(chr) && REPAN_PRIV(is_word_char)(context->result, chr)) {
                continue;
            }

            context->error = REPAN_ERR_NAME_EXPECTED;
            context->pattern = chr_start;
            return NULL;
        }

        length++;

        if (context->options & REPAN_PARSE_UTF) {
            if (REPAN_PRIV(u_match_property)(chr, REPAN_UP_ID_CONTINUE)) {
                continue;
            }
        }
        else if (REPAN_PRIV(is_word_char)(context->result, chr)) {
            continue;
        }

        context->error = REPAN_ERR_INVALID_NAME_CHAR;
        context->pattern = chr_start;
        return NULL;
    }

    if (first_char) {
        context->error = REPAN_ERR_NAME_EXPECTED;
        context->pattern = start;
        return NULL;
    }

    if (length >= REPAN_RESOURCE_MAX) {
        context->error = REPAN_ERR_LENGTH_LIMIT;
        context->pattern = start;
        return NULL;
    }

    if (length == pattern - start) {
        /* No \u escape sequence was encountered. */
        context->pattern = pattern + 1;
        return REPAN_PRIV(string_add)(&context->result->bracket_names, start, length, &context->error);
    }

    string = (uint32_t*)malloc(length * sizeof(uint32_t));
    pattern = start;
    start = string;

    while (REPAN_TRUE) {
        if (*pattern == REPAN_CHAR_GREATER_THAN_SIGN) {
            break;
        }

        if (pattern[0] == REPAN_CHAR_BACKSLASH && pattern[1] == REPAN_CHAR_u) {
            pattern += 2;
            context->pattern = pattern;

            *string++ = parse_unicode_escape(context, locals);
            REPAN_ASSERT(context->error == REPAN_SUCCESS && pattern < context->pattern);

            pattern = context->pattern;
            continue;
        }

        *string++ = *pattern++;
    }

    REPAN_ASSERT(string - start == length);

    name = REPAN_PRIV(string_add)(&context->result->bracket_names, start, length, &context->error);

    free(start);
    context->pattern = pattern + 1;
    return name;
}

static void parse_reference(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t *start;
    repan_string *name = NULL;
    repan_reference_node *reference_node;

    REPAN_ASSERT(context->pattern[-1] == REPAN_CHAR_k);

    if (*context->pattern != REPAN_CHAR_LESS_THAN_SIGN) {
        return;
    }

    context->pattern++;
    start = context->pattern;

    name = parse_name(context, locals);

    if (name == NULL) {
        return;
    }

    if ((name->length_and_flags & (REPAN_STRING_DEFINED | REPAN_STRING_REFERENCED)) == 0) {
        /* A new, not yet defined string. */
        repan_undefined_name *undefined_name = REPAN_ALLOC(repan_undefined_name, context->result);

        if (undefined_name == NULL) {
            context->error = REPAN_ERR_NO_MEMORY;
            return;
        }

        undefined_name->next = context->undefined_names;
        undefined_name->start = start;
        undefined_name->name = name;

        context->undefined_names = undefined_name;
    }

    name->length_and_flags |= REPAN_STRING_REFERENCED;

    reference_node = REPAN_ALLOC(repan_reference_node, context->result);

    if (reference_node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        return;
    }

    reference_node->next_node = NULL;
    reference_node->type = REPAN_NAMED_BACK_REFERENCE_NODE;
    reference_node->u.name = name;

    locals->prev_node = (repan_prev_node*)locals->last_node;
    locals->last_node->next_node = (repan_node*)reference_node;
    locals->last_node = (repan_node*)reference_node;
    return;
}

static int parse_may_backref(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t *pattern = context->pattern;
    repan_reference_node *reference_node;

    uint32_t backref_num = REPAN_PRIV(parse_decimal)(&pattern);

    /* Includes REPAN_DECIMAL_INF check. */
    if (backref_num > locals->capture_count) {
        return REPAN_FALSE;
    }

    reference_node = REPAN_ALLOC(repan_reference_node, context->result);

    if (reference_node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        context->pattern--;
        return REPAN_TRUE;
    }

    reference_node->next_node = NULL;
    reference_node->type = REPAN_BACK_REFERENCE_NODE;
    reference_node->u.number = backref_num;

    locals->prev_node = (repan_prev_node*)locals->last_node;
    locals->last_node->next_node = (repan_node*)reference_node;
    locals->last_node = (repan_node*)reference_node;

    context->pattern = pattern;
    return REPAN_TRUE;
}

static void parse_character(repan_parser_context *context, repan_parser_locals *locals, uint32_t current_char)
{
    uint32_t *char_start = context->pattern;
    uint8_t node_type = REPAN_CHAR_NODE;
    uint8_t node_sub_type = 0;
    size_t size;
    repan_node *node;

    context->pattern++;

    switch (current_char) {
    case REPAN_CHAR_BACKSLASH:
        current_char = *context->pattern++;

        switch (current_char) {
        case REPAN_CHAR_b:
            node_type = REPAN_ASSERT_WORD_BOUNDARY_NODE;
            break;
        case REPAN_CHAR_B:
            node_type = REPAN_ASSERT_NOT_WORD_BOUNDARY_NODE;
            break;
        case REPAN_CHAR_c:
            if (REPAN_IS_CASELESS_LATIN(*context->pattern)) {
                current_char = (*context->pattern & 0x1f);
                context->pattern++;
            }
            break;
        case REPAN_CHAR_d:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_CTYPE_DIGIT;
            break;
        case REPAN_CHAR_D:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_CTYPE_DIGIT;
            break;
        case REPAN_CHAR_f:
            current_char = REPAN_ESC_f;
            break;
        case REPAN_CHAR_k:
            parse_reference(context, locals);
            return;
        case REPAN_CHAR_n:
            current_char = REPAN_ESC_n;
            break;
        case REPAN_CHAR_p:
        case REPAN_CHAR_P:
            if (*context->pattern == REPAN_CHAR_LEFT_BRACE) {
                node = REPAN_PRIV(u_parse_property)(context, REPAN_TRUE);
                if (node != NULL) {
                    REPAN_ASSERT(context->error == REPAN_SUCCESS);
                    locals->prev_node = (repan_prev_node*)locals->last_node;
                    locals->last_node->next_node = node;
                    locals->last_node = node;
                }
                return;
            }
            break;
        case REPAN_CHAR_r:
            current_char = REPAN_ESC_r;
            break;
        case REPAN_CHAR_s:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_CTYPE_SPACE;
            break;
        case REPAN_CHAR_S:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_CTYPE_SPACE;
            break;
        case REPAN_CHAR_t:
            current_char = REPAN_ESC_t;
            break;
        case REPAN_CHAR_u:
            current_char = parse_unicode_escape(context, locals);
            if (context->error != REPAN_SUCCESS) {
                return;
            }
            break;
        case REPAN_CHAR_v:
            current_char = 0x0b;
            break;
        case REPAN_CHAR_w:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_CTYPE_WORD;
            break;
        case REPAN_CHAR_W:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_CTYPE_WORD;
            break;
        case REPAN_CHAR_x:
            current_char = parse_hex(context, 2);

            if (current_char == UINT32_MAX) {
                current_char = REPAN_CHAR_x;
            }
            break;
        case REPAN_CHAR_0:
            current_char = parse_oct(context, 2);
            break;
        case REPAN_CHAR_1:
        case REPAN_CHAR_2:
        case REPAN_CHAR_3:
        case REPAN_CHAR_4:
        case REPAN_CHAR_5:
        case REPAN_CHAR_6:
        case REPAN_CHAR_7:
        case REPAN_CHAR_8:
        case REPAN_CHAR_9:
            context->pattern--;
            if (parse_may_backref(context, locals)) {
                return;
            }
            if (current_char <= REPAN_CHAR_7) {
                current_char = parse_oct(context, 3);
            }
            break;
        }
        break;
    case REPAN_CHAR_CIRCUMFLEX_ACCENT:
        node_type = REPAN_ASSERT_CIRCUMFLEX_NODE;
        node_sub_type = (uint8_t)(context->options & REPAN_PARSE_MULTILINE);
        break;
    case REPAN_CHAR_DOLLAR:
        node_type = REPAN_ASSERT_DOLLAR_NODE;
        node_sub_type = (uint8_t)(context->options & REPAN_PARSE_MULTILINE);
        break;
    case REPAN_CHAR_DOT:
        node_type = REPAN_DOT_NODE;
        node_sub_type = (uint8_t)(context->options & REPAN_PARSE_DOT_ANY);
        break;
    }

    size = (node_type == REPAN_CHAR_NODE) ? sizeof(repan_char_node) : sizeof(repan_node);
    node = (repan_node*)REPAN_PRIV(alloc)(context->result, size);

    if (node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        context->pattern = char_start;
        return;
    }

    node->next_node = NULL;
    node->type = node_type;
    node->sub_type = node_sub_type;

    if (node_type == REPAN_CHAR_NODE) {
        repan_char_node *char_node = (repan_char_node*)node;

        char_node->caseless = (uint8_t)(context->options & REPAN_PARSE_CASELESS);
        char_node->chr = current_char;
    }

    locals->prev_node = (repan_prev_node*)locals->last_node;
    locals->last_node->next_node = node;
    locals->last_node = node;
}

static repan_prev_node *parse_range_add_char(repan_parser_context *context, repan_prev_node *prev_node, uint32_t chr)
{
    repan_char_node *char_node = REPAN_ALLOC(repan_char_node, context->result);

    if (char_node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        return NULL;
    }

    char_node->next_node = NULL;
    char_node->type = REPAN_CHAR_NODE;
    char_node->caseless = (uint8_t)(context->options & REPAN_PARSE_CASELESS);
    char_node->chr = chr;

    prev_node->next_node = (repan_node*)char_node;
    return (repan_prev_node*)char_node;
}

enum {
    REPAN_NO_RANGE,
    REPAN_RANGE_VALID,
    REPAN_RANGE_INVALID,
};

static void parse_char_range(repan_parser_context *context, repan_parser_locals *locals)
{
    repan_char_class_node *char_class_node = REPAN_ALLOC(repan_char_class_node, context->result);
    repan_prev_node *prev_node;
    uint32_t *class_start, *pattern, *pattern_end;
    uint8_t caseless = (uint8_t)(context->options & REPAN_PARSE_CASELESS);
    int range_mode = REPAN_NO_RANGE;
    uint32_t *range_start = NULL;
    uint32_t range_left = 0;

    if (char_class_node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        return;
    }

    char_class_node->next_node = NULL;
    char_class_node->type = REPAN_CHAR_CLASS_NODE;
    char_class_node->sub_type = REPAN_NORMAL_CLASS;
    char_class_node->caseless = caseless;
    char_class_node->node_list.next_node = NULL;

    locals->last_node->next_node = (repan_node*)char_class_node;
    locals->last_node = (repan_node*)char_class_node;

    pattern = context->pattern + 1;
    if (*pattern == REPAN_CHAR_CIRCUMFLEX_ACCENT) {
        char_class_node->sub_type = REPAN_NEGATED_CLASS;
        pattern++;
    }

    prev_node = &char_class_node->node_list;
    class_start = context->pattern;
    pattern_end = context->pattern_end;

    while (*pattern != REPAN_CHAR_RIGHT_SQUARE_BRACKET) {
        uint32_t current_char;
        size_t size;
        repan_node *node;
        uint8_t node_type = REPAN_CHAR_NODE;
        uint8_t node_sub_type = 0;

        context->pattern = pattern;

        if (pattern >= pattern_end) {
            context->error = REPAN_ERR_UNTERMINATED_CHAR_CLASS;
            context->pattern = class_start;
            return;
        }

        if (range_mode == REPAN_NO_RANGE) {
            range_start = pattern;
        }

        current_char = *pattern++;

        if (current_char == REPAN_CHAR_BACKSLASH) {
            current_char = *pattern++;

            switch (current_char) {
            case REPAN_CHAR_b:
                current_char = REPAN_ESC_b;
                break;
            case REPAN_CHAR_c:
                if (REPAN_IS_CASELESS_LATIN(*pattern)
                        || REPAN_IS_DECIMAL_DIGIT(*pattern)
                        || *pattern == REPAN_CHAR_UNDERSCORE) {
                    current_char = (*pattern & 0x1f);
                    pattern++;
                }
                break;
            case REPAN_CHAR_d:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_CTYPE_DIGIT;
                break;
            case REPAN_CHAR_D:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_CTYPE_DIGIT;
                break;
            case REPAN_CHAR_f:
                current_char = REPAN_ESC_f;
                break;
            case REPAN_CHAR_n:
                current_char = REPAN_ESC_n;
                break;
            case REPAN_CHAR_p:
            case REPAN_CHAR_P:
                if (*pattern == REPAN_CHAR_LEFT_BRACE) {
                    context->pattern = pattern;
                    node = REPAN_PRIV(u_parse_property)(context, REPAN_TRUE);

                    if (context->error != REPAN_SUCCESS) {
                        return;
                    }

                    REPAN_ASSERT(node != NULL);
                    prev_node->next_node = node;
                    prev_node = (repan_prev_node*)node;

                    pattern = context->pattern;
                    continue;
                }
                break;
            case REPAN_CHAR_r:
                current_char = REPAN_ESC_r;
                break;
            case REPAN_CHAR_s:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_CTYPE_SPACE;
                break;
            case REPAN_CHAR_S:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_CTYPE_SPACE;
                break;
            case REPAN_CHAR_t:
                current_char = REPAN_ESC_t;
                break;
            case REPAN_CHAR_u:
                context->pattern = pattern;
                current_char = parse_unicode_escape(context, locals);
                if (context->error != REPAN_SUCCESS) {
                    return;
                }
                pattern = context->pattern;
                break;
            case REPAN_CHAR_v:
                current_char = 0x0b;
                break;
            case REPAN_CHAR_w:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_CTYPE_WORD;
                break;
            case REPAN_CHAR_W:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_CTYPE_WORD;
                break;
            case REPAN_CHAR_x:
                context->pattern = pattern;
                current_char = parse_hex(context, 2);
                pattern = context->pattern;

                if (current_char == UINT32_MAX) {
                    current_char = REPAN_CHAR_x;
                }
                break;
            case REPAN_CHAR_0:
            case REPAN_CHAR_1:
            case REPAN_CHAR_2:
            case REPAN_CHAR_3:
            case REPAN_CHAR_4:
            case REPAN_CHAR_5:
            case REPAN_CHAR_6:
            case REPAN_CHAR_7:
                context->pattern = pattern - 1;
                current_char = parse_oct(context, 3);
                pattern = context->pattern;
                return;

            default:
                break;
            }
        }

        size = (node_type == REPAN_CHAR_NODE) ? sizeof(repan_char_node) : sizeof(repan_node);

        if (range_mode == REPAN_NO_RANGE) {
            if (pattern[0] == REPAN_CHAR_MINUS && pattern[1] != REPAN_CHAR_RIGHT_SQUARE_BRACKET) {
                pattern++;

                if (node_type == REPAN_CHAR_NODE) {
                    range_mode = REPAN_RANGE_VALID;
                    range_left = current_char;
                    continue;
                }

                range_mode = REPAN_RANGE_INVALID;
            }
        }
        else {
            if (range_mode == REPAN_RANGE_INVALID) {
                prev_node = parse_range_add_char(context, prev_node, REPAN_CHAR_MINUS);
                if (prev_node == NULL) {
                    return;
                }
            }
            else if (node_type != REPAN_CHAR_NODE) {
                prev_node = parse_range_add_char(context, prev_node, range_left);
                if (prev_node == NULL) {
                    return;
                }
                prev_node = parse_range_add_char(context, prev_node, REPAN_CHAR_MINUS);
                if (prev_node == NULL) {
                    return;
                }
            }
            else if (current_char < range_left) {
                context->error = REPAN_ERR_RANGE_OUT_OF_ORDER;
                context->pattern = range_start;
                return;
            }
            else if (current_char > range_left) {
                size = sizeof(repan_char_range_node);
                node_type = REPAN_CHAR_RANGE_NODE;
            }

            range_mode = REPAN_NO_RANGE;
        }

        node = (repan_node*)REPAN_PRIV(alloc)(context->result, size);

        if (node == NULL) {
            context->error = REPAN_ERR_NO_MEMORY;
            return;
        }

        node->next_node = NULL;
        node->type = node_type;
        node->sub_type = node_sub_type;

        if (node_type == REPAN_CHAR_NODE) {
            repan_char_node *char_node = (repan_char_node*)node;

            char_node->caseless = caseless;
            char_node->chr = current_char;
        }
        else if (node_type == REPAN_CHAR_RANGE_NODE) {
            repan_char_range_node *char_range_node = (repan_char_range_node*)node;

            char_range_node->caseless = caseless;
            char_range_node->chrs[0] = range_left;
            char_range_node->chrs[1] = current_char;
        }

        prev_node->next_node = node;
        prev_node = (repan_prev_node*)node;
    }

    context->pattern = pattern + 1;
}
