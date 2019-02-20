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

enum {
    REPAN_PARSE_PCRE,
    REPAN_PARSE_JAVASCRIPT,
};

/* ------------------------------------------------------------------------------- */
/*  Support for uint8_t types.                                                     */
/* ------------------------------------------------------------------------------- */

static uint8_t *repan_check_u8_utf(uint8_t *chr, uint8_t *end)
{
    uint8_t c;

    REPAN_ASSERT(chr < end);

    c = chr[0];
    if (c < 0x80)
        return chr + 1;

    /* More than two byte long characters must be >= 0x80. */
    if (c < 0xc2)
        return NULL;

    if ((chr + 1 >= end) || (chr[1] & 0xc0) != 0x80)
        return NULL;

    if (c < 0xe0) {
        return chr + 2;
    }

    if ((chr + 2 >= end) || ((chr[2] & 0xc0) != 0x80))
        return NULL;

    if (c < 0xf0) {
        /* Three byte long characters must be >= 0x800. */
        if (c == 0xe0 && chr[1] < 0xa0)
            return NULL;

        /* Three byte long characters must not use the 0xd800-0xdfff range. */
        if (c == 0xed && chr[1] >= 0xa0)
            return NULL;

        return chr + 3;
    }

    if (c > 0xf4)
        return NULL;

    /* Four byte long characters must be <= 0x10ffff. */
    if (c == 0xf4 && chr[1] >= 0x90)
        return NULL;

    if ((chr + 3 >= end) || ((chr[3] & 0xc0) != 0x80))
        return NULL;

    /* Four byte long characters must be >= 0x10000. */
    if (c == 0xf0 && (chr[1] < 0x90))
        return NULL;

    return chr + 4;
}

static uint8_t *repan_decode_u8_utf(uint8_t *chr, uint32_t *dst)
{
    uint32_t c = chr[0];
    uint32_t d;

    if (c < 0x80) {
        *dst = *chr;
        return chr + 1;
    }

    d = ((uint32_t)chr[1] & 0x3f);

    if (c < 0xe0) {
        *dst = ((c & 0x1f) << 6) | d;
        return chr + 2;
    }

    d = (d << 6) | ((uint32_t)chr[2] & 0x3f);

    if (c < 0xf0) {
        *dst = ((c & 0x0f) << 12) | d;
        return chr + 3;
    }

    d = (d << 6) | ((uint32_t)chr[3] & 0x3f);
    *dst = ((c & 0x07) << 18) | d;
    return chr + 4;
}

static uint8_t *repan_get_u8_utf_length(uint8_t *chr)
{
    uint8_t c = chr[0];

    if (c < 0x80)
        return chr + 1;
    if (c < 0xe0)
        return chr + 2;
    if (c < 0xf0)
        return chr + 3;
    return chr + 4;
}

static repan_pattern *parse_u8(uint8_t *pattern, size_t length, uint32_t options,
        repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset, int mode)
{
    repan_parser_context context;
    uint8_t *src;
    uint32_t *dst, *dst_end;

    if (options & REPAN_PARSE_UTF) {
        uint8_t *src_end = pattern + length;

        src = pattern;
        length = 0;

        while (src < src_end) {
            uint8_t *next_src = repan_check_u8_utf(src, src_end);
            if (next_src == NULL) {
                if (error != NULL)
                    *error = REPAN_ERR_INVALID_UTF_CHAR;
                if (error_offset != NULL)
                    *error_offset = (size_t)(src - pattern);
                return NULL;
            }

            src = next_src;
            length++;
        }
    }

    dst = (uint32_t *)malloc(sizeof(uint32_t) * (length + 1));

    if (dst == NULL) {
        if (error != NULL)
            *error = REPAN_ERR_NO_MEMORY;
        if (error_offset != NULL)
            *error_offset = 0;
        return NULL;
    }

    context.pattern_start = dst;
    dst_end = dst + length;
    context.pattern_end = dst_end;

    src = pattern;
    if (!(options & REPAN_PARSE_UTF)) {
        context.char_max = 0xff;

        while (dst < dst_end) {
            *dst++ = *src++;
        }
    }
    else {
        context.char_max = 0x10ffff;

        while (dst < dst_end) {
            src = repan_decode_u8_utf(src, dst);
            dst++;
        }
    }

    *dst = REPAN_CHAR_NUL;

    context.options = options;
    context.extra_opts = extra_opts;

    switch (mode) {
    case REPAN_PARSE_PCRE:
        REPAN_PRIV(repan_parse_pcre)(&context);
        break;
    default:
        REPAN_ASSERT(mode == REPAN_PARSE_JAVASCRIPT);
        REPAN_PRIV(repan_parse_javascript)(&context);
        break;
    }

    if (context.result)
        return context.result;

    if (error != NULL)
        *error = context.error;

    if (error_offset != NULL) {
        size_t offset = (size_t)(context.pattern - context.pattern_start);

        if (!(options & REPAN_PARSE_UTF)) {
            *error_offset = offset;
        }
        else {
            src = pattern;
            while (offset > 0) {
                src = repan_get_u8_utf_length(src);
                offset--;
            }
            *error_offset = (size_t)(src - pattern);
        }
    }

    return NULL;
}

repan_pattern *repan_parse_pcre_u8(uint8_t *pattern, size_t length, uint32_t options,
     repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset)
{
    return parse_u8(pattern, length, options, extra_opts, error, error_offset, REPAN_PARSE_PCRE);
}

repan_pattern *repan_parse_javascript_u8(uint8_t *pattern, size_t length, uint32_t options,
     repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset)
{
    return parse_u8(pattern, length, options, extra_opts, error, error_offset, REPAN_PARSE_JAVASCRIPT);
}

/* ------------------------------------------------------------------------------- */
/*  Support for uint8_t types.                                                     */
/* ------------------------------------------------------------------------------- */

static uint16_t *repan_check_u16_utf(uint16_t *chr, uint16_t *end)
{
    uint16_t c;

    REPAN_ASSERT(chr < end);

    c = chr[0];
    if (c < 0xd800 || c >= 0xe000)
        return chr + 1;

    /* Low-surrogate is not allowed here. */
    if (c >= 0xdc00)
        return NULL;

    if ((chr + 1 >= end) || (chr[1] & 0xfc00) != 0xdc00)
        return NULL;

    return chr + 2;
}

static uint16_t *repan_decode_u16_utf(uint16_t *chr, uint32_t *dst)
{
    uint32_t c = chr[0];

    if (c < 0xd800 || c >= 0xe000) {
        *dst = *chr;
        return chr + 1;
    }

    *dst = ((c & 0x03ff) << 10) | (((uint32_t)chr[1]) & 0x03ff);
    return chr + 2;
}

static uint16_t *repan_get_u16_utf_length(uint16_t *chr)
{
    if ((chr[0] & 0xfc00) != 0xd800)
        return chr + 1;
    return chr + 2;
}

static repan_pattern *parse_u16(uint16_t *pattern, size_t length, uint32_t options,
        repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset, int mode)
{
    repan_parser_context context;
    uint16_t *src;
    uint32_t *dst, *dst_end;

    if (options & REPAN_PARSE_UTF) {
        uint16_t *src_end = pattern + length;

        src = pattern;
        length = 0;

        while (src < src_end) {
            uint16_t *next_src = repan_check_u16_utf(src, src_end);
            if (next_src == NULL) {
                if (error != NULL)
                    *error = REPAN_ERR_INVALID_UTF_CHAR;
                if (error_offset != NULL)
                    *error_offset = (size_t)(src - pattern);
                return NULL;
            }

            src = next_src;
            length++;
        }
    }

    dst = (uint32_t *)malloc(sizeof(uint32_t) * (length + 1));

    if (dst == NULL) {
        if (error != NULL)
            *error = REPAN_ERR_NO_MEMORY;
        if (error_offset != NULL)
            *error_offset = 0;
        return NULL;
    }

    context.pattern_start = dst;
    dst_end = dst + length;
    context.pattern_end = dst_end;

    src = pattern;
    if (!(options & REPAN_PARSE_UTF)) {
        context.char_max = 0xffff;

        while (dst < dst_end) {
            *dst++ = *src++;
        }
    }
    else {
        context.char_max = 0x10ffff;

        while (dst < dst_end) {
            src = repan_decode_u16_utf(src, dst);
            dst++;
        }
    }

    *dst = REPAN_CHAR_NUL;

    context.options = options;
    context.extra_opts = extra_opts;

    switch (mode) {
    case REPAN_PARSE_PCRE:
        REPAN_PRIV(repan_parse_pcre)(&context);
        break;
    default:
        REPAN_ASSERT(mode == REPAN_PARSE_JAVASCRIPT);
        REPAN_PRIV(repan_parse_javascript)(&context);
        break;
    }

    if (context.result)
        return context.result;

    if (error != NULL)
        *error = context.error;
    if (error_offset != NULL) {
        size_t offset = (size_t)(context.pattern - context.pattern_start);

        if (!(options & REPAN_PARSE_UTF)) {
            *error_offset = offset;
        }
        else {
            src = pattern;
            while (offset > 0) {
                src = repan_get_u16_utf_length(src);
                offset--;
            }
            *error_offset = (size_t)(src - pattern);
        }
    }

    return NULL;
}

repan_pattern *repan_parse_pcre_u16(uint16_t *pattern, size_t length, uint32_t options,
     repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset)
{
    return parse_u16(pattern, length, options, extra_opts, error, error_offset, REPAN_PARSE_PCRE);
}

repan_pattern *repan_parse_javascript_u16(uint16_t *pattern, size_t length, uint32_t options,
     repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset)
{
    return parse_u16(pattern, length, options, extra_opts, error, error_offset, REPAN_PARSE_JAVASCRIPT);
}
