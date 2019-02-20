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
#include "to_string.h"

static void count_char_generic(repan_to_string_context *context, uint32_t chr)
{
    context->char_count++;
}

/* ------------------------------------------------------------------------------- */
/*  Support for uint8_t types.                                                     */
/* ------------------------------------------------------------------------------- */

static void write_char_u8(repan_to_string_context *context, uint32_t chr)
{
    uint8_t *dst = (uint8_t*)(context->result);

    dst[0] = chr;
    context->result = (void*)(dst + 1);
}

static void count_char_u8_utf(repan_to_string_context *context, uint32_t chr)
{
    size_t len;

    if (chr < 0x80)
        len = 1;
    else if (chr < 0x800)
        len = 2;
    else if (chr < 0x10000)
        len = 3;
    else
        len = 4;

    context->char_count += len;
}

static void write_char_u8_utf(repan_to_string_context *context, uint32_t chr)
{
    uint8_t *dst = (uint8_t*)(context->result);

    if (chr < 0x80) {
        *dst = (uint8_t)chr;
    } else {
        if (chr < 0x800) {
            *dst++ = (uint8_t)((chr >> 6) | 0xc0);
        }
        else {
            if (chr < 0x10000) {
                *dst++ = (uint8_t)((chr >> 12) | 0xe0);
            }
            else {
                *dst++ = (uint8_t)((chr >> 18) | 0xf0);
                *dst++ = (uint8_t)(((chr >> 12) & 0x3f) | 0x80);
            }

            *dst++ = (uint8_t)(((chr >> 6) & 0x3f) | 0x80);
        }

        *dst = (uint8_t)((chr & 0x3f) | 0x80);
    }

    context->result = (void*)(dst + 1);
}

uint8_t *repan_to_string_u8(repan_pattern *pattern, uint32_t options,
        repan_to_string_extra_opts *extra_opts, size_t *length, uint32_t *error)
{
    repan_to_string_context context;

    context.pattern = pattern;
    context.options = options;
    context.extra_opts = extra_opts;
    context.char_size = 1;

    if (!(options & REPAN_TO_STRING_UTF)) {
        context.char_max = 0xff;

        REPAN_PRIV(repan_to_string)(&context, count_char_generic, write_char_u8);
    }
    else {
        context.char_max = 0x10ffff;

        REPAN_PRIV(repan_to_string)(&context, count_char_u8_utf, write_char_u8_utf);
    }

    if (error != NULL)
        *error = context.error;

    if (context.error != REPAN_SUCCESS) {
        if (length != NULL)
            *length = 0;
        return NULL;
    }

    if (length != NULL)
        *length = context.char_count - 1;
    return (uint8_t*)context.result;
}

/* ------------------------------------------------------------------------------- */
/*  Support for uint16_t types.                                                    */
/* ------------------------------------------------------------------------------- */

static void write_char_u16(repan_to_string_context *context, uint32_t chr)
{
    uint16_t *dst = (uint16_t*)(context->result);

    dst[0] = chr;
    context->result = (void*)(dst + 1);
}

static void count_char_u16_utf(repan_to_string_context *context, uint32_t chr)
{
    context->char_count += 1 + (chr >= 0x10000);
}

static void write_char_u16_utf(repan_to_string_context *context, uint32_t chr)
{
    uint16_t *dst = (uint16_t*)(context->result);

    if (chr < 0x10000) {
        *dst = chr;
    }
    else {
        chr -= 0x10000;
        *dst++ = 0xd800 | (chr >> 10);
        *dst = 0xdc00 | (chr & 0x3ff);
    }

    context->result = (void*)(dst + 1);
}

uint16_t *repan_to_string_u16(repan_pattern *pattern, uint32_t options,
        repan_to_string_extra_opts *extra_opts, size_t *length, uint32_t *error)
{
    repan_to_string_context context;

    context.pattern = pattern;
    context.options = options;
    context.extra_opts = extra_opts;
    context.char_size = 2;

    if (!(options & REPAN_TO_STRING_UTF)) {
        context.char_max = 0xffff;

        REPAN_PRIV(repan_to_string)(&context, count_char_generic, write_char_u16);
    }
    else {
        context.char_max = 0x10ffff;

        REPAN_PRIV(repan_to_string)(&context, count_char_u16_utf, write_char_u16_utf);
    }

    if (error != NULL)
        *error = context.error;

    if (context.error != REPAN_SUCCESS) {
        if (length != NULL)
            *length = 0;
        return NULL;
    }

    if (length != NULL)
        *length = context.char_count - 1;
    return (uint16_t*)context.result;
}
