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

#include "repan.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static void help(char *name)
{
    printf("Usage: %s max\n", name);
    printf("  max: character table size, must be between 1 and 256\n");
}

typedef int (*is_ctype)(int chr);

static int ctype_isdigit(int chr)
{
    return isdigit(chr);
}

static int ctype_isspace(int chr)
{
    return isspace(chr);
}

static int ctype_isword(int chr)
{
    return isalnum(chr) || chr == '_';
}

static int ctype_isalnum(int chr)
{
    return isalnum(chr);
}

static int ctype_isalpha(int chr)
{
    return isalpha(chr);
}

static int ctype_isascii(int chr)
{
    return chr >= 0 && chr <= 127;
}

static int ctype_isblank(int chr)
{
    return chr == ' ' || chr == '\t';
}

static int ctype_iscntrl(int chr)
{
    return iscntrl(chr);
}

static int ctype_isgraph(int chr)
{
    return isgraph(chr);
}

static int ctype_islower(int chr)
{
    return islower(chr);
}

static int ctype_isprint(int chr)
{
    return isprint(chr);
}

static int ctype_ispunct(int chr)
{
    return ispunct(chr);
}

static int ctype_isupper(int chr)
{
    return isupper(chr);
}

static int ctype_isxdigit(int chr)
{
    return isxdigit(chr);
}

static void generate_table(char *name, char *prefix, int max, is_ctype is_ctype_cb)
{
    int i, count, start, column;
    char buffer[32];

    start = -1;
    count = 0;
    for (i = 0; i < max + 1; i++) {
        if (i < max && is_ctype_cb(i)) {
            if (start == -1) {
                start = i;
            }
            continue;
        }
        else if (start == -1) {
            continue;
        }

        count++;

        if (i > start + 1) {
            count++;
        }

        start = -1;
    }

    if (prefix != NULL) {
        printf("static const uint32_t ctype_%s_%s[%d] = {\n    %d", prefix, name, count + 1, count);
    }
    else {
        printf("static const uint32_t ctype_%s[%d] = {\n    %d", name, count + 1, count);
    }

    start = -1;
    column = 100;

    for (i = 0; i < max + 1; i++) {
        if (i < max && is_ctype_cb(i)) {
            if (start == -1) {
                start = i;
            }
            continue;
        }
        else if (start == -1) {
            continue;
        }

        if (i > start + 1) {
            printf(",");
            column += 1;

            if (column >= 100) {
                printf("\n   ");
                column = 3;
            }

            sprintf(buffer, " R$ | 0x%x", start);

            column += (int)strlen(buffer);
            printf("%s", buffer);
        }

        printf(",");
        column += 1;

        if (column >= 100) {
            printf("\n   ");
            column = 3;
        }

        sprintf(buffer, " 0x%x", i - 1);

        column += (int)strlen(buffer);
        printf("%s", buffer);

        start = -1;
    }

    printf("\n};\n\n");
}

int main(int argc, char *argv[])
{
    int max, i, column, first;
    char buffer[32];
    char *ptr;
    char *prefix = NULL;

    if (argc <= 1) {
        help(argv[0]);
        return -1;
    }

    ptr = argv[1];
    max = 0;

    if (*ptr == '\0' || *ptr == '0') {
        help(argv[0]);
        return -1;
    }

    while (*ptr >= '0' && *ptr <= '9' && max < 100) {
        max = max * 10 + (int)*ptr - '0';
        ptr++;
    }

    if (*ptr != '\0' || max < 1 || max > 256) {
        help(argv[0]);
        return -1;
    }

    if (argc >= 3 && argv[2][0] != '\0') {
        prefix = argv[2];
    }

    printf("/* WARNING: These structures are automatically generated, do not edit! */\n\n");

    if (prefix != NULL) {
        printf("static const uint8_t case_folding_%s[%d] = {", prefix, max);
    }
    else {
        printf("static const uint8_t case_folding[%d] = {", max);
    }

    column = 100;
    first = 1;

    for (i = 0; i < max; i++) {
        int other_case = toupper(i);

        if (i == other_case) {
            other_case = tolower(i);
        }

        if (!first) {
            printf(",");
            column += 1;
        }
        first = 0;

        if (column >= 100) {
            printf("\n   ");
            column = 3;
        }

        sprintf(buffer, " %d", other_case);

        column += (int)strlen(buffer);
        printf("%s", buffer);
    }

    printf("\n};\n\n#define R$ REPAN_RANGE_START\n\n");

    generate_table("is_digit", prefix, max, ctype_isdigit);
    generate_table("is_space", prefix, max, ctype_isspace);
    generate_table("is_word", prefix, max, ctype_isword);
    generate_table("is_alnum", prefix, max, ctype_isalnum);
    generate_table("is_alpha", prefix, max, ctype_isalpha);
    generate_table("is_ascii", prefix, max, ctype_isascii);
    generate_table("is_blank", prefix, max, ctype_isblank);
    generate_table("is_cntrl", prefix, max, ctype_iscntrl);
    generate_table("is_graph", prefix, max, ctype_isgraph);
    generate_table("is_lower", prefix, max, ctype_islower);
    generate_table("is_print", prefix, max, ctype_isprint);
    generate_table("is_punct", prefix, max, ctype_ispunct);
    generate_table("is_upper", prefix, max, ctype_isupper);
    generate_table("is_xdigit", prefix, max, ctype_isxdigit);

    printf("#undef R$\n\n/* End of generated structures. */\n\n");

    return 0;
}
