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

/* WARNING: These structures are automatically generated, do not edit! */

static const uint8_t case_folding[128] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
    27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
    107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 94, 95,
    96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88,
    89, 90, 123, 124, 125, 126, 127
};

#define R$ REPAN_RANGE_START

static const uint32_t ctype_is_digit[3] = {
    2,
    R$ | 0x30, 0x39
};

static const uint32_t ctype_is_space[4] = {
    3,
    R$ | 0x9, 0xd, 0x20
};

static const uint32_t ctype_is_word[8] = {
    7,
    R$ | 0x30, 0x39, R$ | 0x41, 0x5a, 0x5f, R$ | 0x61, 0x7a
};

static const uint32_t ctype_is_alnum[7] = {
    6,
    R$ | 0x30, 0x39, R$ | 0x41, 0x5a, R$ | 0x61, 0x7a
};

static const uint32_t ctype_is_alpha[5] = {
    4,
    R$ | 0x41, 0x5a, R$ | 0x61, 0x7a
};

static const uint32_t ctype_is_ascii[3] = {
    2,
    R$ | 0x0, 0x7f
};

static const uint32_t ctype_is_blank[3] = {
    2,
    0x9, 0x20
};

static const uint32_t ctype_is_cntrl[4] = {
    3,
    R$ | 0x0, 0x1f, 0x7f
};

static const uint32_t ctype_is_graph[3] = {
    2,
    R$ | 0x21, 0x7e
};

static const uint32_t ctype_is_lower[3] = {
    2,
    R$ | 0x61, 0x7a
};

static const uint32_t ctype_is_print[3] = {
    2,
    R$ | 0x20, 0x7e
};

static const uint32_t ctype_is_punct[9] = {
    8,
    R$ | 0x21, 0x2f, R$ | 0x3a, 0x40, R$ | 0x5b, 0x60, R$ | 0x7b, 0x7e
};

static const uint32_t ctype_is_upper[3] = {
    2,
    R$ | 0x41, 0x5a
};

static const uint32_t ctype_is_xdigit[7] = {
    6,
    R$ | 0x30, 0x39, R$ | 0x41, 0x46, R$ | 0x61, 0x66
};

#undef R$

/* End of generated structures. */

#define R$ REPAN_RANGE_START

/* These structures are fixed in PCRE. */

static const uint32_t ctype_is_hspace[] = {
    10,
    0x09, 0x20, 0xa0, 0x1680, 0x180e, R$ | 0x2000, 0x200a, 0x202f, 0x205f, 0x3000
};

static const uint32_t ctype_is_vspace[] = {
    5,
    R$ | 0x0a, 0x0d, 0x85, 0x2028, 0x2029
};

#undef R$

typedef const uint32_t *ctype_data;

static const ctype_data default_ctype_list[] = {
    ctype_is_digit,
    ctype_is_space,
    ctype_is_hspace,
    ctype_is_vspace,
    ctype_is_word,
    ctype_is_alnum,
    ctype_is_alpha,
    ctype_is_ascii,
    ctype_is_blank,
    ctype_is_cntrl,
    ctype_is_graph,
    ctype_is_lower,
    ctype_is_print,
    ctype_is_punct,
    ctype_is_upper,
    ctype_is_xdigit,
    ctype_is_vspace /* REPAN_CTYPE_NEWLINE */
};
