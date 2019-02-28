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

static uint32_t parse_hex(repan_parser_context *context, int max_chars)
{
    uint32_t value = 0;

    do {
        uint32_t current_char = *context->pattern;

        if (REPAN_IS_DECIMAL_DIGIT(current_char)) {
            value = (value << 4) | (current_char - REPAN_CHAR_0);
        }
        else if (current_char >= REPAN_CHAR_A && current_char <= REPAN_CHAR_F) {
            value = (value << 4) | (current_char - (REPAN_CHAR_A - 10));
        }
        else if (current_char >= REPAN_CHAR_a && current_char <= REPAN_CHAR_f) {
            value = (value << 4) | (current_char - (REPAN_CHAR_a - 10));
        }
        else {
            return value;
        }

        context->pattern++;
    } while (--max_chars > 0);

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

enum {
    REPAN_REF_NUMBER_NOT_ALLOWED,
    REPAN_REF_SIGNED_NUMBER_ALLOWED,
    REPAN_REF_UNSIGNED_NUMBER_ALLOWED,
};

static void parse_reference(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t *pattern = context->pattern;
    repan_string *name = NULL;
    uint32_t ref_num = 0;
    repan_reference_node *reference_node;
    uint32_t terminator_char = REPAN_CHAR_NUL;
    uint32_t terminator_error = 0;
    uint8_t node_type = REPAN_BACK_REFERENCE_NODE;
    int number_mode = REPAN_REF_SIGNED_NUMBER_ALLOWED;
    int name_allowed = REPAN_TRUE;

    REPAN_ASSERT(REPAN_NAMED_BACK_REFERENCE_NODE == REPAN_BACK_REFERENCE_NODE + 1
            && REPAN_NAMED_RECURSE_NODE == REPAN_RECURSE_NODE + 1);

    switch (pattern[-1]) {
    case REPAN_CHAR_g:
        switch (*pattern) {
        case REPAN_CHAR_LEFT_BRACE:
            pattern++;
            terminator_char = REPAN_CHAR_RIGHT_BRACE;
            terminator_error = REPAN_ERR_RIGHT_BRACE_EXPECTED;
            break;
        case REPAN_CHAR_LESS_THAN_SIGN:
            terminator_char = REPAN_CHAR_GREATER_THAN_SIGN;
            terminator_error = REPAN_ERR_GREATER_THAN_SIGN_EXPECTED;
            node_type = REPAN_RECURSE_NODE;
            pattern++;
            break;
        case REPAN_CHAR_APOSTROPHE:
            terminator_char = REPAN_CHAR_APOSTROPHE;
            terminator_error = REPAN_ERR_APOSTROPHE_EXPECTED;
            node_type = REPAN_RECURSE_NODE;
            pattern++;
            break;
        default:
            number_mode = REPAN_REF_UNSIGNED_NUMBER_ALLOWED;
            name_allowed = REPAN_FALSE;
            break;
        }
        break;

    case REPAN_CHAR_k:
        switch (*pattern) {
        case REPAN_CHAR_LEFT_BRACE:
            terminator_char = REPAN_CHAR_RIGHT_BRACE;
            break;
        case REPAN_CHAR_LESS_THAN_SIGN:
            terminator_char = REPAN_CHAR_GREATER_THAN_SIGN;
            break;
        case REPAN_CHAR_APOSTROPHE:
            terminator_char = REPAN_CHAR_APOSTROPHE;
            break;
        default:
            context->error = REPAN_ERR_INVALID_K_SEQUENCE;
            context->pattern -= 2;
            return;
        }

        number_mode = REPAN_REF_NUMBER_NOT_ALLOWED;
        pattern++;
        break;

    case REPAN_CHAR_QUESTION_MARK:
        terminator_char = REPAN_CHAR_RIGHT_BRACKET;
        terminator_error = REPAN_ERR_RIGHT_BRACKET_EXPECTED;
        node_type = REPAN_RECURSE_NODE;

        switch (pattern[0]) {
        case REPAN_CHAR_R:
            name_allowed = REPAN_FALSE;
            pattern++;
            if (pattern[0] != REPAN_CHAR_RIGHT_BRACKET) {
                context->error = REPAN_ERR_RIGHT_BRACKET_EXPECTED;
                context->pattern = pattern;
                return;
            }
            /* Fallthrough. */
        case REPAN_CHAR_AMPERSAND:
            number_mode = REPAN_REF_NUMBER_NOT_ALLOWED;
            pattern++;
            break;
        case REPAN_CHAR_P:
            REPAN_ASSERT(pattern[1] == REPAN_CHAR_GREATER_THAN_SIGN || pattern[1] == REPAN_CHAR_EQUALS_SIGN);

            pattern += 2;
            number_mode = REPAN_REF_NUMBER_NOT_ALLOWED;

            if (pattern[-1] == REPAN_CHAR_EQUALS_SIGN) {
                node_type = REPAN_BACK_REFERENCE_NODE;
            }
            break;
        default:
            name_allowed = REPAN_FALSE;
            break;
        }
        break;

    default:
        REPAN_ASSERT(0);
        break;
    }

    context->pattern = pattern;

    if (number_mode != REPAN_REF_NUMBER_NOT_ALLOWED) {
        int sign = 0;

        if (number_mode == REPAN_REF_SIGNED_NUMBER_ALLOWED) {
            switch (*pattern) {
            case REPAN_CHAR_PLUS:
                sign = 1;
                pattern++;
                break;
            case REPAN_CHAR_MINUS:
                sign = -1;
                pattern++;
                break;
            }
        }

        if (REPAN_IS_DECIMAL_DIGIT(*pattern)) {
            ref_num = parse_decimal(&pattern);

            if (ref_num == 0 && sign != 0) {
                context->error = REPAN_ERR_SIGNED_ZERO_IS_NOT_ALLOWED;
                return;
            }

            if (ref_num == REPAN_DECIMAL_INF || (ref_num == 0 && node_type != REPAN_RECURSE_NODE)) {
                context->error = REPAN_ERR_CAPTURING_BRACKET_NOT_EXIST;
                return;
            }

            if (terminator_char != REPAN_CHAR_NUL) {
                if (*pattern != terminator_char) {
                    context->error = terminator_error;
                    context->pattern = pattern;
                    return;
                }
                pattern++;
            }

            if (sign == -1) {
                ref_num--;
                if (ref_num >= locals->capture_count) {
                    context->error = REPAN_ERR_CAPTURING_BRACKET_NOT_EXIST;
                    return;
                }
                ref_num = locals->capture_count - ref_num;
            }
            else {
                if (sign == 1) {
                    ref_num += locals->capture_count;
                }

                if (ref_num > locals->highest_capture_ref) {
                    locals->highest_capture_ref = ref_num;
                    locals->highest_capture_ref_start = context->pattern;
                }
            }

            name_allowed = REPAN_FALSE;
        }
        else if (!name_allowed) {
            REPAN_ASSERT(number_mode == REPAN_REF_UNSIGNED_NUMBER_ALLOWED);
            context->error = REPAN_ERR_UNSIGNED_INTEGER_REQUIRED;
            return;
        }
        else if (sign != 0) {
            context->error = REPAN_ERR_NAME_EXPECTED;
            return;
        }
    }

    if (name_allowed) {
        if (REPAN_IS_DECIMAL_DIGIT(*pattern)) {
            context->error = REPAN_ERR_NAME_EXPECTED;
            return;
        }

        REPAN_ASSERT(terminator_char != REPAN_CHAR_NUL);

        while (REPAN_PRIV(is_word_char)(context->result, *pattern)) {
            pattern++;
        }

        if (pattern == context->pattern) {
            context->error = REPAN_ERR_NAME_EXPECTED;
            return;
        }

        if (*pattern != terminator_char) {
            context->error = REPAN_ERR_INVALID_NAME_CHAR;
            context->pattern = pattern;
            return;
        }

        name = parse_add_name_reference(context, context->pattern, (size_t)(pattern - context->pattern));

        if (name == NULL) {
            return;
        }

        pattern++;
        node_type = (uint8_t)(node_type + 1);
    }

    reference_node = REPAN_ALLOC(repan_reference_node, context->result);

    if (reference_node == NULL) {
        context->error = REPAN_ERR_NO_MEMORY;
        return;
    }

    reference_node->next_node = NULL;
    reference_node->type = node_type;

    if (name != NULL) {
        reference_node->u.name = name;
    }
    else {
        reference_node->u.number = ref_num;
    }

    locals->prev_node = (repan_prev_node*)locals->last_node;
    locals->last_node->next_node = (repan_node*)reference_node;
    locals->last_node = (repan_node*)reference_node;

    context->pattern = pattern;
    return;
}

static uint8_t parse_u_property(repan_parser_context *context)
{
    uint32_t *pattern = context->pattern;
    uint32_t data;
    uint32_t base = 0;

    REPAN_ASSERT(pattern[-1] == REPAN_CHAR_p || pattern[-1] == REPAN_CHAR_P);

    if (pattern[-1] == REPAN_CHAR_P) {
        base = REPAN_NEG_U_PROPERTY_CLASS;
    }

    if (*pattern != REPAN_CHAR_LEFT_BRACE) {
        uint32_t chr = *pattern++;

        if (!REPAN_IS_CASELESS_LATIN(chr)) {
            context->error = REPAN_ERR_INVALID_P_SEQUENCE;
            context->pattern -= 2;
            return 0;
        }

        data = REPAN_PRIV(find_u_property)(&chr, 1);
    }
    else {
        pattern++;

        while (REPAN_IS_CASELESS_LATIN(*pattern) || *pattern == REPAN_CHAR_UNDERSCORE
                || *pattern == REPAN_CHAR_AMPERSAND) {
            pattern++;
        }

        if (pattern == context->pattern) {
            context->error = REPAN_ERR_INVALID_P_SEQUENCE;
            context->pattern -= 2;
            return 0;
        }

        if (*pattern != REPAN_CHAR_RIGHT_BRACE) {
            context->error = REPAN_ERR_RIGHT_BRACE_EXPECTED;
            context->pattern = pattern;
            return 0;
        }

        data = REPAN_PRIV(find_u_property)(context->pattern + 1, pattern - context->pattern - 1);
        pattern++;
    }

    if (data == 0) {
        context->error = REPAN_ERR_UNKNOWN_PROPERTY;
        context->pattern -= 2;
        return 0;
    }
    context->pattern = pattern;
    return (uint8_t)((data & REPAN_U_PROPERTY_TYPE_MASK) + base);
}

static int parse_may_backref(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t *pattern = context->pattern;
    repan_reference_node *reference_node;

    int is_backref = (*pattern == REPAN_CHAR_8 || *pattern == REPAN_CHAR_9);
    uint32_t backref_num = parse_decimal(&pattern);

    if (!is_backref && backref_num >= 10 && backref_num > locals->capture_count) {
        return REPAN_FALSE;
    }

    if (backref_num == REPAN_DECIMAL_INF) {
        context->error = REPAN_ERR_CAPTURING_BRACKET_NOT_EXIST;
        context->pattern--;
        return REPAN_TRUE;
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

    if (backref_num > locals->highest_capture_ref) {
        locals->highest_capture_ref = backref_num;
        locals->highest_capture_ref_start = context->pattern - 1;
    }

    context->pattern = pattern;
    return REPAN_TRUE;
}

static void parse_raw_chars(repan_parser_context *context, repan_parser_locals *locals)
{
    uint32_t *pattern = context->pattern;
    uint32_t *pattern_end = context->pattern_end;
    uint8_t caseless = (uint8_t)(locals->current.modifiers & REPAN_MODIFIER_CASELESS);

    while (pattern[0] != REPAN_CHAR_BACKSLASH || pattern[1] != REPAN_CHAR_E) {
        if (pattern >= pattern_end) {
            context->error = REPAN_ERR_UNTERMINATED_RAW_CHARS;
            context->pattern -= 2;
            return;
        }

        repan_char_node *char_node = REPAN_ALLOC(repan_char_node, context->result);

        if (char_node == NULL) {
            context->error = REPAN_ERR_NO_MEMORY;
            context->pattern = pattern;
            return;
        }

        char_node->next_node = NULL;
        char_node->type = REPAN_CHAR_NODE;
        char_node->caseless = caseless;
        char_node->chr = *pattern;

        locals->prev_node = (repan_prev_node*)locals->last_node;
        locals->last_node->next_node = (repan_node*)char_node;
        locals->last_node = (repan_node*)char_node;

        pattern++;
    }

    context->pattern = pattern + 2;
}

static uint32_t parse_hex_escape(repan_parser_context *context)
{
    uint32_t *start, *digits_start;
    uint32_t result;

    if (context->pattern[-1] == REPAN_CHAR_x) {
        if (context->pattern[0] != REPAN_CHAR_LEFT_BRACE) {
            result = parse_hex(context, 2);
            return result;
        }

        start = context->pattern - 2;
    }
    else {
        REPAN_ASSERT(context->pattern[-1] == REPAN_CHAR_U && context->pattern[0] == REPAN_CHAR_PLUS);
        start = context->pattern - 4;
    }

    context->pattern++;

    digits_start = context->pattern;

    while (*context->pattern == REPAN_CHAR_0) {
        context->pattern++;
    }

    result = parse_hex(context, 8);

    if (context->pattern == digits_start) {
        context->error = REPAN_ERR_HEXADECIMAL_NUMBER_REQUIRED;
        context->pattern = start;
        return 0;
    }

    if (*context->pattern != REPAN_CHAR_RIGHT_BRACE) {
        context->error = REPAN_ERR_RIGHT_BRACE_EXPECTED;
        return 0;
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

    context->pattern++;
    return result;
}

static uint32_t parse_oct_escape(repan_parser_context *context)
{
    uint32_t *start = context->pattern - 2;
    uint32_t result;

    REPAN_ASSERT(context->pattern[-1] == REPAN_CHAR_o);

    if (context->pattern[0] != REPAN_CHAR_LEFT_BRACE) {
        context->error = REPAN_ERR_OCTAL_NUMBER_REQUIRED;
        context->pattern = start;
        return 0;
    }

    context->pattern++;

    while (*context->pattern == REPAN_CHAR_0) {
        context->pattern++;
    }

    result = parse_oct(context, 9 + (*context->pattern <= REPAN_CHAR_3));

    if (context->pattern == start + 3) {
        context->error = REPAN_ERR_OCTAL_NUMBER_REQUIRED;
        context->pattern = start;
        return 0;
    }

    if (*context->pattern != REPAN_CHAR_RIGHT_BRACE) {
        context->error = REPAN_ERR_RIGHT_BRACE_EXPECTED;
        return 0;
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

    context->pattern++;
    return result;
}

static void parse_character(repan_parser_context *context, repan_parser_locals *locals, uint32_t current_char)
{
    uint8_t node_type = REPAN_CHAR_NODE;
    uint8_t node_sub_type = 0;
    size_t size;
    repan_node *node;

    context->pattern++;

    if (locals->current.modifiers & REPAN_MODIFIER_EXTENDED) {
        if (REPAN_PRIV(is_space)(context->result, current_char)) {
            while (REPAN_PRIV(is_space)(context->result, *context->pattern)) {
                context->pattern++;
            }
            return;
        }
        if (current_char == REPAN_CHAR_HASHMARK) {
            uint32_t *pattern = context->pattern;
            uint32_t *pattern_end = context->pattern_end;

            while (pattern < pattern_end && !REPAN_PRIV(is_newline)(context->result, *pattern)) {
                pattern++;
            }
            context->pattern = pattern;
            return;
        }
    }

    switch (current_char) {
    case REPAN_CHAR_BACKSLASH:
        current_char = *context->pattern++;

        switch (current_char) {
        case REPAN_CHAR_a:
            current_char = REPAN_ESC_a;
            break;
        case REPAN_CHAR_A:
            node_type = REPAN_ASSERT_SUBJECT_START_NODE;
            break;
        case REPAN_CHAR_b:
            node_type = REPAN_ASSERT_WORD_BOUNDARY_NODE;
            break;
        case REPAN_CHAR_B:
            node_type = REPAN_ASSERT_NOT_WORD_BOUNDARY_NODE;
            break;
        case REPAN_CHAR_c:
            current_char = *context->pattern++;

            if (current_char < 0x20 || current_char > 0x7e) {
                context->error = REPAN_ERR_INVALID_C_SEQUENCE;
                context->pattern -= 3;
                return;
            }

            if (REPAN_IS_LOWERCASE_LATIN(current_char)) {
                current_char -= REPAN_CHAR_a - REPAN_CHAR_A;
            }

            current_char ^= 0x40;
            break;
        case REPAN_CHAR_C:
            node_type = REPAN_CODE_UNIT_NODE;
            break;
        case REPAN_CHAR_d:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_DECIMAL_DIGIT_CLASS;
            break;
        case REPAN_CHAR_D:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_DECIMAL_DIGIT_CLASS;
            break;
        case REPAN_CHAR_e:
            current_char = REPAN_ESC_e;
            break;
        case REPAN_CHAR_E:
            return;
        case REPAN_CHAR_f:
            current_char = REPAN_ESC_f;
            break;
        case REPAN_CHAR_g:
            parse_reference(context, locals);
            return;
        case REPAN_CHAR_G:
            node_type = REPAN_ASSERT_MATCH_START_NODE;
            break;
        case REPAN_CHAR_h:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_HORIZONTAL_SPACE_CLASS;
            break;
        case REPAN_CHAR_H:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_HORIZONTAL_SPACE_CLASS;
            break;
        case REPAN_CHAR_k:
            parse_reference(context, locals);
            return;
        case REPAN_CHAR_K:
            node_type = REPAN_SET_MATCH_START_NODE;
            break;
        case REPAN_CHAR_n:
            current_char = REPAN_ESC_n;
            break;
        case REPAN_CHAR_Q:
            parse_raw_chars(context, locals);
            return;
        case REPAN_CHAR_p:
        case REPAN_CHAR_P:
            node_sub_type = parse_u_property(context);

            if (context->error != REPAN_SUCCESS) {
                return;
            }
            node_type = REPAN_U_PROPERTY_NODE;
            break;
        case REPAN_CHAR_r:
            current_char = REPAN_ESC_r;
            break;
        case REPAN_CHAR_R:
            node_type = REPAN_NEWLINE_NODE;
            break;
        case REPAN_CHAR_s:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_SPACE_CLASS;
            break;
        case REPAN_CHAR_S:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_SPACE_CLASS;
            break;
        case REPAN_CHAR_t:
            current_char = REPAN_ESC_t;
            break;
        case REPAN_CHAR_v:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_VERTICAL_SPACE_CLASS;
            break;
        case REPAN_CHAR_V:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_VERTICAL_SPACE_CLASS;
            break;
        case REPAN_CHAR_w:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_WORD_CHAR_CLASS;
            break;
        case REPAN_CHAR_W:
            node_type = REPAN_PERL_CLASS_NODE;
            node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_WORD_CHAR_CLASS;
            break;
        case REPAN_CHAR_X:
            node_type = REPAN_GRAPHEME_CLUSTER_NODE;
            break;
        case REPAN_CHAR_z:
            node_type = REPAN_ASSERT_SUBJECT_END_NODE;
            break;
        case REPAN_CHAR_Z:
            node_type = REPAN_ASSERT_SUBJECT_END_NEWLINE_NODE;
            break;
        case REPAN_CHAR_N:
            if (*context->pattern != REPAN_CHAR_LEFT_BRACE) {
                node_type = REPAN_DOT_NODE;
                break;
            }

            /* \N{U+xxxx} */
            if (context->pattern[1] != REPAN_CHAR_U
                    || context->pattern[2] != REPAN_CHAR_PLUS) {
                context->error = REPAN_ERR_UNICODE_NAMES_NOT_SUPPORTED;
                context->pattern -= 2;
                return;
            }
            context->pattern += 2;
            /* Fallthrough. */
        case REPAN_CHAR_x:
            current_char = parse_hex_escape(context);

            if (context->error != REPAN_SUCCESS) {
                return;
            }
            break;
        case REPAN_CHAR_o:
            current_char = parse_oct_escape(context);

            if (context->error != REPAN_SUCCESS) {
                return;
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
            current_char = parse_oct(context, 3);
            break;
        }
        break;
    case REPAN_CHAR_CIRCUMFLEX_ACCENT:
        node_type = REPAN_ASSERT_CIRCUMFLEX_NODE;
        node_sub_type = (locals->current.modifiers & REPAN_MODIFIER_MULTILINE) != 0;
        break;
    case REPAN_CHAR_DOLLAR:
        node_type = REPAN_ASSERT_DOLLAR_NODE;
        node_sub_type = (locals->current.modifiers & REPAN_MODIFIER_MULTILINE) != 0;
        break;
    case REPAN_CHAR_DOT:
        node_type = REPAN_DOT_NODE;
        node_sub_type = (locals->current.modifiers & REPAN_MODIFIER_DOT_ANY) != 0;
        break;
    }

    size = (node_type == REPAN_CHAR_NODE) ? sizeof(repan_char_node) : sizeof(repan_node);
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

        char_node->caseless = (uint8_t)(locals->current.modifiers & REPAN_MODIFIER_CASELESS);
        char_node->chr = current_char;
    }

    locals->prev_node = (repan_prev_node*)locals->last_node;
    locals->last_node->next_node = node;
    locals->last_node = node;
}

static uint32_t parse_posix_class(uint32_t **pattern_start)
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

static void parse_char_range(repan_parser_context *context, repan_parser_locals *locals)
{
    repan_char_class_node *char_class_node = REPAN_ALLOC(repan_char_class_node, context->result);
    repan_prev_node *prev_node;
    uint32_t *class_start, *pattern, *pattern_end;
    uint8_t caseless = (uint8_t)(locals->current.modifiers & REPAN_MODIFIER_CASELESS);
    int in_range = REPAN_FALSE;
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

    do {
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

        if (!in_range) {
            range_start = pattern;
        }

        current_char = *pattern++;

        if (current_char == REPAN_CHAR_BACKSLASH) {
            current_char = *pattern++;

            switch (current_char) {
            case REPAN_CHAR_a:
                current_char = REPAN_ESC_a;
                break;
            case REPAN_CHAR_b:
                current_char = REPAN_ESC_b;
                break;
            case REPAN_CHAR_c:
                current_char = *pattern++;

                if (current_char < 0x20 || current_char > 0x7e) {
                    context->error = REPAN_ERR_INVALID_C_SEQUENCE;
                    return;
                }

                if (REPAN_IS_LOWERCASE_LATIN(current_char)) {
                    current_char -= REPAN_CHAR_a - REPAN_CHAR_A;
                }

                current_char ^= 0x40;
                break;
            case REPAN_CHAR_d:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_DECIMAL_DIGIT_CLASS;
                break;
            case REPAN_CHAR_D:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_DECIMAL_DIGIT_CLASS;
                break;
            case REPAN_CHAR_e:
                current_char = REPAN_ESC_e;
                break;
            case REPAN_CHAR_f:
                current_char = REPAN_ESC_f;
                break;
            case REPAN_CHAR_h:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_HORIZONTAL_SPACE_CLASS;
                break;
            case REPAN_CHAR_H:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_HORIZONTAL_SPACE_CLASS;
                break;
            case REPAN_CHAR_n:
                current_char = REPAN_ESC_n;
                break;
            case REPAN_CHAR_p:
            case REPAN_CHAR_P:
                context->pattern = pattern;
                node_sub_type = parse_u_property(context);

                if (context->error != REPAN_SUCCESS) {
                    return;
                }

                pattern = context->pattern;
                node_type = REPAN_U_PROPERTY_NODE;
                break;
            case REPAN_CHAR_r:
                current_char = REPAN_ESC_r;
                break;
            case REPAN_CHAR_s:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_SPACE_CLASS;
                break;
            case REPAN_CHAR_S:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_SPACE_CLASS;
                break;
            case REPAN_CHAR_t:
                current_char = REPAN_ESC_t;
                break;
            case REPAN_CHAR_v:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_VERTICAL_SPACE_CLASS;
                break;
            case REPAN_CHAR_V:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_VERTICAL_SPACE_CLASS;
                break;
            case REPAN_CHAR_w:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_WORD_CHAR_CLASS;
                break;
            case REPAN_CHAR_W:
                node_type = REPAN_PERL_CLASS_NODE;
                node_sub_type = REPAN_NEG_PERL_CLASS + REPAN_WORD_CHAR_CLASS;
                break;
            case REPAN_CHAR_N:
                if (pattern[0] != REPAN_CHAR_LEFT_BRACE) {
                    context->error = REPAN_ERR_ESCAPE_NOT_ALLOWED_IN_CLASS;
                    context->pattern = pattern - 2;
                    return;
                }

                /* \N{U+xxxx} */
                if (pattern[1] != REPAN_CHAR_U
                        || pattern[2] != REPAN_CHAR_PLUS) {
                    context->error = REPAN_ERR_UNICODE_NAMES_NOT_SUPPORTED;
                    context->pattern = pattern - 2;
                    return;
                }
                pattern += 2;
            /* Fallthrough. */
            case REPAN_CHAR_x:
                context->pattern = pattern;
                current_char = parse_hex_escape(context);

                if (context->error != REPAN_SUCCESS) {
                    return;
                }
                pattern = context->pattern;
                break;
            case REPAN_CHAR_o:
                context->pattern = pattern;
                current_char = parse_oct_escape(context);

                if (context->error != REPAN_SUCCESS) {
                    return;
                }
                pattern = context->pattern;
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
                return;

            default:
                if (REPAN_IS_CASELESS_LATIN(current_char)) {
                    context->error = REPAN_ERR_ESCAPE_NOT_ALLOWED_IN_CLASS;
                    context->pattern = pattern - 2;
                    return;
                }
                break;
            }
        }
        else if (current_char == REPAN_CHAR_LEFT_SQUARE_BRACKET && *pattern == REPAN_CHAR_COLON) {
            uint32_t data = parse_posix_class(&pattern);

            if (data != 0) {
                node_type = (data & REPAN_POSIX_OPT_IS_PERL) ? REPAN_PERL_CLASS_NODE : REPAN_POSIX_CLASS_NODE;
                node_sub_type = (uint8_t)data;
            }
        }
        else if ((locals->current.modifiers & REPAN_MODIFIER_EXTENDED_MORE)
                && (current_char == REPAN_CHAR_SPACE || current_char == REPAN_ESC_t)) {
            continue;
        }

        size = (node_type == REPAN_CHAR_NODE) ? sizeof(repan_char_node) : sizeof(repan_node);

        if (!in_range) {
            if (pattern[0] == REPAN_CHAR_MINUS && pattern[1] != REPAN_CHAR_RIGHT_SQUARE_BRACKET) {
                if (node_type != REPAN_CHAR_NODE) {
                    context->error = REPAN_ERR_INVALID_RANGE;
                    context->pattern = range_start;
                    return;
                }

                in_range = REPAN_TRUE;
                range_left = current_char;
                pattern++;
                continue;
            }
        }
        else {
            in_range = REPAN_FALSE;

            if (node_type != REPAN_CHAR_NODE) {
                context->error = REPAN_ERR_INVALID_RANGE;
                context->pattern = range_start;
                return;
            }

            if (current_char < range_left) {
                context->error = REPAN_ERR_RANGE_OUT_OF_ORDER;
                context->pattern = range_start;
                return;
            }

            if (current_char > range_left) {
                size = sizeof(repan_char_range_node);
                node_type = REPAN_CHAR_RANGE_NODE;
            }
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
    } while (*pattern != REPAN_CHAR_RIGHT_SQUARE_BRACKET);

    context->pattern = pattern + 1;
}
