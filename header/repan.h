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

#ifndef REPAN_H
#define REPAN_H

#include <stddef.h>
#include <stdint.h>

struct repan_pattern_struct;
typedef struct repan_pattern_struct repan_pattern;

struct repan_parse_extra_opts_struct;
typedef struct repan_parse_extra_opts_struct repan_parse_extra_opts;

struct repan_to_string_extra_opts_struct;
typedef struct repan_to_string_extra_opts_struct repan_to_string_extra_opts;

/* Error codes. */
enum {
    REPAN_SUCCESS = 0,
    REPAN_ERR_NO_MEMORY,
    REPAN_ERR_CAPTURE_LIMIT,
    REPAN_ERR_LENGTH_LIMIT,
    REPAN_ERR_LIST_LIMIT,
    REPAN_ERR_INVALID_UTF_CHAR,
    REPAN_ERR_INVALID_BRACKET,
    REPAN_ERR_UNTERMINATED_BRACKET,
    REPAN_ERR_UNTERMINATED_CHAR_CLASS,
    REPAN_ERR_UNEXPECTED_BRACKET_CLOSE,
    REPAN_ERR_NOTHING_TO_REPEAT,
    REPAN_ERR_TOO_BIG_QUANTIFIER,
    REPAN_ERR_QUANTIFIER_OUT_OF_ORDER,
    REPAN_ERR_TOO_LARGE_CHARACTER,
    REPAN_ERR_RIGHT_BRACKET_EXPECTED,
    REPAN_ERR_RIGHT_BRACE_EXPECTED,
    REPAN_ERR_GREATER_THAN_SIGN_EXPECTED,
    REPAN_ERR_EQUALS_SIGN_EXPECTED,
    REPAN_ERR_APOSTROPHE_EXPECTED,
    REPAN_ERR_COLON_EXPECTED,
    REPAN_ERR_BACKSLASH_EXPECTED,
    REPAN_ERR_SIGNED_INTEGER_REQUIRED,
    REPAN_ERR_UNSIGNED_INTEGER_REQUIRED,
    REPAN_ERR_SIGNED_ZERO_IS_NOT_ALLOWED,
    REPAN_ERR_HEXADECIMAL_NUMBER_REQUIRED,
    REPAN_ERR_OCTAL_NUMBER_REQUIRED,
    REPAN_ERR_CAPTURING_BRACKET_NOT_EXIST,
    REPAN_ERR_NAME_EXPECTED,
    REPAN_ERR_INVALID_NAME_CHAR,
    REPAN_ERR_ESCAPE_NOT_ALLOWED_IN_CLASS,
    REPAN_ERR_INVALID_RANGE,
    REPAN_ERR_RANGE_OUT_OF_ORDER,
    REPAN_ERR_TOO_MANY_ALTERNATIVES,
    REPAN_ERR_DEFINE_HAS_ALTERNATIVES,
    REPAN_ERR_INVALID_COND_ASSERT,
    REPAN_ERR_INVALID_K_SEQUENCE,
    REPAN_ERR_INVALID_C_SEQUENCE,
    REPAN_ERR_INVALID_P_SEQUENCE,
    REPAN_ERR_INVALID_U_SEQUENCE,
    REPAN_ERR_UNKNOWN_PROPERTY,
    REPAN_ERR_UNKNOWN_VERB,
    REPAN_ERR_MARK_ARGUMENT_EXPECTED,
    REPAN_ERR_UNKNOWN_OPTION,
    REPAN_ERR_UNTERMINATED_OPTION_LIST,
    REPAN_ERR_INVALID_HYPEN_IN_OPTION_LIST,
    REPAN_ERR_UNICODE_NAMES_NOT_SUPPORTED,
    REPAN_ERR_UNTERMINATED_RAW_CHARS,
    REPAN_ERR_UNTERMINATED_COMMENT,
    REPAN_ERR_POSIX_INVALID_QUANTIFIER,
    REPAN_ERR_GLOB_INVALID_ASTERISK,
};

/* Flags for any repan_parse... functions. */
#define REPAN_PARSE_UTF 0x00000001
   /* Modifier: i */
#define REPAN_PARSE_CASELESS 0x00000002
   /* Modifier: m */
#define REPAN_PARSE_MULTILINE 0x00000004
   /* Modifier: s */
#define REPAN_PARSE_DOT_ANY 0x00000008

/* Flags for repan_parse_pcre... functions. */
   /* Modifier: n */
#define REPAN_PARSE_PCRE_NO_AUTOCAPTURE 0x00000100
   /* Modifier: x */
#define REPAN_PARSE_PCRE_EXTENDED 0x00000200
   /* Modifier: xx */
#define REPAN_PARSE_PCRE_EXTENDED_MORE 0x00000400

repan_pattern *repan_parse_pcre_u8(uint8_t *pattern, size_t length, uint32_t options,
     repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset);
repan_pattern *repan_parse_pcre_u16(uint16_t *pattern, size_t length, uint32_t options,
     repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset);

repan_pattern *repan_parse_javascript_u8(uint8_t *pattern, size_t length, uint32_t options,
     repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset);
repan_pattern *repan_parse_javascript_u16(uint16_t *pattern, size_t length, uint32_t options,
     repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset);

/* Flags for repan_parse_posix... functions. */
   /* Basic regular expressions (default: extended) */
#define REPAN_PARSE_POSIX_BASIC 0x00000100

repan_pattern *repan_parse_posix_u8(uint8_t *pattern, size_t length, uint32_t options,
     repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset);
repan_pattern *repan_parse_posix_u16(uint16_t *pattern, size_t length, uint32_t options,
     repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset);

repan_pattern *repan_parse_glob_u8(uint8_t *pattern, size_t length, uint32_t options,
     repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset);
repan_pattern *repan_parse_glob_u16(uint16_t *pattern, size_t length, uint32_t options,
     repan_parse_extra_opts *extra_opts, uint32_t *error, size_t *error_offset);

/* Flags for repan_to_string... functions. */
#define REPAN_TO_STRING_UTF 0x1
#define REPAN_TO_STRING_ASCII 0x2

uint8_t *repan_to_string_pcre_u8(repan_pattern *pattern, uint32_t options,
    repan_to_string_extra_opts *extra_opts, size_t *length, uint32_t *error);
uint16_t *repan_to_string_pcre_u16(repan_pattern *pattern, uint32_t options,
    repan_to_string_extra_opts *extra_opts, size_t *length, uint32_t *error);

/* Optimizations. */

uint32_t repan_uncapture(repan_pattern *pattern);
uint32_t repan_flatten(repan_pattern *pattern);

/* Other functions. */

void repan_pattern_free(repan_pattern *pattern);
const char *repan_get_error_message(uint32_t error);

#endif /* REPAN_H */
