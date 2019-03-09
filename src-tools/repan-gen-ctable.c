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

void help(char *name)
{
    printf("Usage: %s max\n", name);
    printf("  max: character table size, must be between 1 and 256\n");
}

int main(int argc, char *argv[])
{
    int max, i, column, first;
    char buffer[32];
    char *ptr;

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

    printf("/* WARNING: These two structures are automatically generated, do not edit! */\n\n");
    printf("const uint8_t case_folding[%d] = {", max);

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

    printf("\n};\n\nconst uint16_t character_types[%d] = {", max);

    column = 100;
    first = 1;

    for (i = 0; i < max; i++) {
        int flags = 0;

        if (iscntrl(i)) {
            flags |= REPAN_CTYPE_FLAG_CNTRL;
        }

        if (isdigit(i)) {
            flags |= REPAN_CTYPE_FLAG_DIGIT;
        }

        if (isgraph(i)) {
            flags |= REPAN_CTYPE_FLAG_GRAPH;
        }

        if (islower(i)) {
            flags |= REPAN_CTYPE_FLAG_LOWER;
        }

        if (ispunct(i)) {
            flags |= REPAN_CTYPE_FLAG_PUNCT;
        }

        if (isspace(i)) {
            flags |= REPAN_CTYPE_FLAG_SPACE;
        }

        if (isupper(i)) {
            flags |= REPAN_CTYPE_FLAG_UPPER;
        }

        if (isxdigit(i)) {
            flags |= REPAN_CTYPE_FLAG_XDIGIT;
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

        sprintf(buffer, " 0x%x", flags);

        column += (int)strlen(buffer);
        printf("%s", buffer);
    }

    printf("\n};\n");

    return 0;
}
