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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define IS_NEWLINE(chr) ((chr) == '\r' || (chr) == '\n')
#define IS_SPACE(chr) ((chr) == ' ' || (chr) == '\t')
#define IS_HEX(chr) (((chr) >= '0' && (chr) <= '9') || ((chr) >= 'a' && (chr) <= 'f'))
#define IS_DEC(chr) ((chr) >= '0' && (chr) <= '9')

typedef struct {
    repan_pattern *pattern;
    void *pattern_buffer;
    size_t pattern_size;
    size_t pattern_buffer_capacity;
    uint32_t options;
    int line;
    int char_size;
    char end_char;
} test_context;

static void print_str(const char *str, const char *end)
{
    while (str < end) {
        putchar(*str++);
    }
}

static void print_str_u8(const uint8_t *str, size_t length, char end_char)
{
    const uint8_t *end = str + length;

    while (str < end) {
        if (str[0] >= 0x7f || str[0] < 0x20) {
            printf("%%%x%s", (int)str[0], IS_HEX(str[1]) ? ";" : "");
        }
        else if (str[0] == '%' && IS_HEX(str[1])) {
            printf("%%;");
        }
        else if (str[0] == end_char) {
            printf("%%%c", end_char);
        }
        else {
            putchar((char)*str);
        }
        str++;
    }
}

static void print_str_u16(const uint16_t *str, size_t length, char end_char)
{
    const uint16_t *end = str + length;

    while (str < end) {
        if (str[0] >= 0x7f || str[0] < 0x20) {
            printf("%%%x%s", (int)str[0], IS_HEX(str[1]) ? ";" : "");
        }
        else if (str[0] == '%' && IS_HEX(str[1])) {
            printf("%%;");
        }
        else if (str[0] == end_char) {
            printf("%%%c", end_char);
        }
        else {
            putchar((char)*str);
        }
        str++;
    }
}

static const char *skip_all_spaces(test_context *context, const char *data)
{
    while (TRUE) {
        if (IS_SPACE(*data)) {
            data++;
            continue;
        }

        if (*data == '\n') {
            context->line++;
            data++;
            continue;
        }

        if (*data == '\r') {
            context->line++;
            data++;
            if (*data == '\n') {
                data++;
            }
            continue;
        }

        return data;
    }
}

static const char *skip_spaces(const char *data)
{
    while (IS_SPACE(*data)) {
        data++;
    }
    return data;
}

static int process_pattern_u8(test_context *context, const uint8_t *src, const uint8_t *end, uint8_t end_char)
{
    size_t pattern_max_size = end - src;
    uint8_t *dst;

    if (pattern_max_size > context->pattern_buffer_capacity) {
        /* Align up to 256, and increase by 256. */
        context->pattern_buffer_capacity = (pattern_max_size + 2 * 256 - 1) & ~(256 - 1);
        void *new_pattern_buffer = malloc(context->pattern_buffer_capacity);

        if (new_pattern_buffer == NULL) {
            return FALSE;
        }

        free(context->pattern_buffer);
        context->pattern_buffer = new_pattern_buffer;
    }

    dst = (uint8_t*)context->pattern_buffer;

    while (src < end) {
        if (src + 1 >= end) {
            *dst++ = *src;
            break;
        }

        if (src[0] == '%') {
            if (src[1] == ';') {
                *dst++ = '%';
                src += 2;
                continue;
            }

            if (src[1] == end_char) {
                *dst++ = end_char;
                src += 2;
                continue;
            }

            if (IS_HEX(src[1])) {
                uint32_t chr = 0;
                src++;

                do {
                    if (IS_DEC(*src)) {
                        chr = chr * 16 + (*src++) - '0';
                    }
                    else {
                        chr = chr * 16 + (*src++) - ('a' - 10);
                    }
                } while (IS_HEX(*src));

                if (*src == ';') {
                    src++;
                }

                *dst++ = (uint8_t)chr;
                continue;
            }
        }
        *dst++ = *src++;
    }

    context->pattern_size = dst - (uint8_t*)context->pattern_buffer;
    return TRUE;
}

static int process_pattern_u16(test_context *context, const uint8_t *src, const uint8_t *end, uint16_t end_char)
{
    size_t pattern_max_size = (end - src) * 2;
    uint16_t *dst;

    if (pattern_max_size > context->pattern_buffer_capacity) {
        /* Align up to 256, and increase by 256. */
        context->pattern_buffer_capacity = (pattern_max_size + 2 * 256 - 1) & ~(256 - 1);
        void *new_pattern_buffer = malloc(context->pattern_buffer_capacity);

        if (new_pattern_buffer == NULL) {
            return FALSE;
        }

        free(context->pattern_buffer);
        context->pattern_buffer = new_pattern_buffer;
    }

    dst = (uint16_t*)context->pattern_buffer;

    while (src < end) {
        if (src + 1 >= end) {
            *dst++ = *src;
            break;
        }

        if (src[0] == '%') {
            if (src[1] == ';') {
                *dst++ = '%';
                src += 2;
                continue;
            }

            if (src[1] == end_char) {
                *dst++ = end_char;
                src += 2;
                continue;
            }

            if (IS_HEX(src[1])) {
                uint32_t chr = 0;
                src++;

                do {
                    if (IS_DEC(*src)) {
                        chr = chr * 16 + (*src++) - '0';
                    }
                    else {
                        chr = chr * 16 + (*src++) - ('a' - 10);
                    }
                } while (IS_HEX(*src));

                if (*src == ';') {
                    src++;
                }

                *dst++ = (uint16_t)chr;
                continue;
            }
        }
        *dst++ = *src++;
    }

    context->pattern_size = dst - (uint16_t*)context->pattern_buffer;
    return TRUE;
}

static int execute_commands(test_context *context, const char *data, const char *end, int line)
{
    uint32_t print_options = 0;
    uint32_t error;

    while (TRUE) {
        const char *command_start = data;
        size_t length;

        /* Read command */
        while (data < end && !IS_SPACE(*data) && !IS_NEWLINE(*data) && *data != ',' && *data != '=') {
            data++;
        }

        length = data - command_start;

        if (length == 5 && memcmp(command_start, "print", 5) == 0) {
            void *string_result;
            size_t length;

            if (context->options & REPAN_PARSE_UTF) {
                print_options |= REPAN_TO_STRING_UTF;
            }

            if (context->char_size == 8) {
                string_result = (void*)repan_to_string_pcre_u8(context->pattern, print_options, NULL, &length, &error);
            }
            else {
                string_result = (void*)repan_to_string_pcre_u16(context->pattern, print_options, NULL, &length, &error);
            }

            if (string_result == NULL) {
                printf("    Pattern to string command failed: %s\n", repan_get_error_message(error));
            }
            else {
                char end_char = context->end_char;

                printf("    %c", end_char);
                if (context->char_size == 8) {
                    print_str_u8((uint8_t*)string_result, length, end_char);
                }
                else {
                    print_str_u16((uint16_t*)string_result, length, end_char);
                }
                printf("%c\n", end_char);

                free(string_result);
            }
        }
        else if (length == 5 && memcmp(command_start, "ascii", 5) == 0) {
            print_options |= REPAN_TO_STRING_ASCII;
        }
        else if (length == 9 && memcmp(command_start, "uncapture", 9) == 0) {
            uint32_t error = repan_opt_uncapture(context->pattern, 0);
            if (error != REPAN_SUCCESS) {
                printf("Uncapture error: %s\n", repan_get_error_message(error));
                return 0;
            }
        }
        else if (length == 7 && memcmp(command_start, "flatten", 7) == 0) {
            uint32_t error = repan_opt_flatten(context->pattern, 0);
            if (error != REPAN_SUCCESS) {
                printf("Flatten error: %s\n", repan_get_error_message(error));
                return 0;
            }
        }
        else if (length == 18 && memcmp(command_start, "merge_alternatives", 18) == 0) {
            uint32_t error = repan_opt_merge_alternatives(context->pattern, 0);
            if (error != REPAN_SUCCESS) {
                printf("Merge prefix error: %s\n", repan_get_error_message(error));
                return 0;
            }
        }
        else {
            printf("\nError: Unknown command \"");
            print_str(command_start, data);
            printf("\" at line %d\n", line);
            return 1;
        }

        data = skip_spaces(data);

        if (*data != ',') {
            if (data < end) {
                printf("\nError: Newline expected at line %d\n", line);
                return 1;
            }
            return 0;
        }

        data++;

        while (data < end) {
            if (data[0] == '\n') {
                line++;
            }
            else if (data[0] == '\r') {
                line++;
                if (data[1] == '\n') {
                    data++;
                }
            }
            else if (!IS_SPACE(data[0])) {
                break;
            }

            data++;
        }
    }
}

enum {
    REPAN_TEST_PARSE_PCRE,
    REPAN_TEST_PARSE_JAVASCRIPT,
    REPAN_TEST_PARSE_POSIX,
    REPAN_TEST_PARSE_GLOB,
};

static int process_commands(test_context *context, const char *data, const char *end)
{
    const char *begin = data;
    uint32_t error;
    size_t error_offset;

    context->char_size = 0;

    while (TRUE) {
        const char *command_start;
        const char *command_end;
        int command_multiline;
        int command_line;
        int mode;

        data = skip_all_spaces(context, data);

        if (data >= end) {
            break;
        }

        if (*data == '#') {
            while (data < end && !IS_NEWLINE(*data)) {
                data++;
            }
            continue;
        }

        /* Process pattern definition. */
        if (*data == '/' || *data == '!' || *data == '"' || *data == '\'' || *data == '@') {
            char end_char = *data++;
            const uint8_t *pattern_start = (const uint8_t*)data;
            const uint8_t *pattern_end;
            int ret_value;

            context->end_char = end_char;

            while (*data != end_char) {
                if (data >= end) {
                    print_str(begin, data);
                    printf("\nError: Unterminated pattern at line %d\n", context->line);
                    return 1;
                }

                if (data[0] == '%' && data[1] == end_char) {
                    data++;
                }
                data++;
            }

            pattern_end = (const uint8_t*)data;
            data++;

            context->options = 0;
            context->char_size = 0;
            mode = REPAN_TEST_PARSE_PCRE;

            while (TRUE) {
                uint32_t new_option = 0;
                char new_option_char = '\0';
                int new_mode = REPAN_TEST_PARSE_PCRE;

                switch (*data) {
                case 'u':
                    new_option = REPAN_PARSE_UTF;
                    new_option_char = 'u';
                    break;
                case 'i':
                    new_option = REPAN_PARSE_CASELESS;
                    new_option_char = 'i';
                    break;
                case 'm':
                    new_option = REPAN_PARSE_MULTILINE;
                    new_option_char = 'm';
                    break;
                case 's':
                    new_option = REPAN_PARSE_DOT_ANY;
                    new_option_char = 's';
                    break;
                case 'n':
                    new_option = REPAN_PARSE_PCRE_NO_AUTOCAPTURE;
                    new_option_char = 'n';
                    break;
                case 'x':
                    new_option = REPAN_PARSE_PCRE_EXTENDED;
                    new_option_char = 'x';
                    break;
                case 'e':
                    new_option = REPAN_PARSE_PCRE_EXTENDED_MORE;
                    new_option_char = 'e';
                    break;
                case '8':
                    if (context->char_size != 0) {
                        print_str(begin, data);
                        printf("\nError: Duplicatted character size (was %d) at line %d\n",
                            context->char_size, context->line);
                        return 1;
                    }
                    context->char_size = 8;
                    data++;
                    continue;
                case '1':
                    if (data[1] != '6') {
                        break;
                    }

                    if (context->char_size != 0) {
                        print_str(begin, data);
                        printf("\nError: Duplicatted character size (was %d) at line %d\n",
                            context->char_size, context->line);
                        return 1;
                    }
                    context->char_size = 16;
                    data += 2;
                    continue;
                case 'J':
                    new_mode = REPAN_TEST_PARSE_JAVASCRIPT;
                    break;
                case 'G':
                    new_mode = REPAN_TEST_PARSE_GLOB;
                    break;
                case 'O':
                    if (data[1] == 'B') {
                        context->options |= REPAN_PARSE_POSIX_BASIC;
                        data++;
                    }
                    new_mode = REPAN_TEST_PARSE_POSIX;
                    break;
                }

                if (new_option_char != '\0') {
                    if (context->options & new_option) {
                        print_str(begin, data);
                        printf("\nError: Duplicated '%c' flag at line %d\n", new_option_char, context->line);
                        return 1;
                    }
                    context->options |= new_option;
                    data++;
                    continue;
                }

                if (new_mode != REPAN_TEST_PARSE_PCRE) {
                    if (mode != REPAN_TEST_PARSE_PCRE) {
                        print_str(begin, data);
                        printf("\nError: Duplicatted parsing mode at line %d\n", context->line);
                        return 1;
                    }
                    mode = new_mode;
                    data++;
                    continue;
                }

                break;
            }

            data = skip_spaces(data);
            if (data < end && !IS_NEWLINE(*data)) {
                print_str(begin, data);
                printf("\nError: Newline expected at line %d\n", context->line);
                return 1;
            }

            if (context->char_size == 0) {
                context->char_size = 8;
            }

            if (context->char_size == 8) {
                ret_value = process_pattern_u8(context, pattern_start, pattern_end, (uint8_t)end_char);
            }
            else {
                ret_value = process_pattern_u16(context, pattern_start, pattern_end, (uint16_t)end_char);
            }

            if (ret_value == FALSE) {
                print_str(begin, data);
                printf("\nError: Cannot allocate memory at line %d\n", context->line);
                return 1;
            }
            continue;
        }

        /* Process pattern commands. */
        if (context->char_size == 0) {
            print_str(begin, data);
            printf("\nError: Missing pattern for commands at line %d\n", context->line);
            return 1;
        }

        if (context->char_size == 8) {
            switch (mode) {
            case REPAN_TEST_PARSE_PCRE:
                context->pattern = repan_parse_pcre_u8((uint8_t*)context->pattern_buffer,
                    context->pattern_size, context->options, NULL, &error, &error_offset);
                break;
            case REPAN_TEST_PARSE_JAVASCRIPT:
                context->pattern = repan_parse_javascript_u8((uint8_t*)context->pattern_buffer,
                    context->pattern_size, context->options, NULL, &error, &error_offset);
                break;
            case REPAN_TEST_PARSE_POSIX:
                context->pattern = repan_parse_posix_u8((uint8_t*)context->pattern_buffer,
                    context->pattern_size, context->options, NULL, &error, &error_offset);
                break;
            default:
                context->pattern = repan_parse_glob_u8((uint8_t*)context->pattern_buffer,
                    context->pattern_size, context->options, NULL, &error, &error_offset);
                break;
            }
        } else {
            switch (mode) {
            case REPAN_TEST_PARSE_PCRE:
                context->pattern = repan_parse_pcre_u16((uint16_t*)context->pattern_buffer,
                    context->pattern_size, context->options, NULL, &error, &error_offset);
                break;
            case REPAN_TEST_PARSE_JAVASCRIPT:
                context->pattern = repan_parse_javascript_u16((uint16_t*)context->pattern_buffer,
                    context->pattern_size, context->options, NULL, &error, &error_offset);
                break;
            case REPAN_TEST_PARSE_POSIX:
                context->pattern = repan_parse_posix_u16((uint16_t*)context->pattern_buffer,
                    context->pattern_size, context->options, NULL, &error, &error_offset);
                break;
            default:
                context->pattern = repan_parse_glob_u16((uint16_t*)context->pattern_buffer,
                    context->pattern_size, context->options, NULL, &error, &error_offset);
                break;
            }
        }

        command_start = data;
        command_multiline = FALSE;
        command_line = context->line;

        while (TRUE) {
            if (data >= end) {
                command_end = data;

                print_str(begin, data);
                printf("\n");
                break;
            }

            if (*data == ',') {
                command_multiline = TRUE;
            }
            else if (IS_NEWLINE(*data)) {
                command_end = data;
                context->line++;

                if (data[0] == '\r' && data[1] == '\n') {
                    data++;
                }

                if (!command_multiline) {
                    data++;
                    print_str(begin, data);
                    break;
                }
            }
            else if (!IS_SPACE(*data)) {
                command_multiline = FALSE;
            }

            data++;
        }

        begin = data;

        if (context->pattern == NULL) {
            char end_char = context->end_char;

            printf("    Pattern parse failed: %s\n        %c", repan_get_error_message(error), end_char);

            if (context->char_size == 8) {
                print_str_u8((uint8_t*)context->pattern_buffer, error_offset, end_char);
            }
            else {
                print_str_u16((uint16_t*)context->pattern_buffer, error_offset, end_char);
            }

            printf("[ HERE ]>>");

            if (context->char_size == 8) {
                print_str_u8((uint8_t*)context->pattern_buffer + error_offset, context->pattern_size - error_offset, end_char);
            }
            else {
                print_str_u16((uint16_t*)context->pattern_buffer + error_offset, context->pattern_size - error_offset, end_char);
            }

            printf("%c\n", end_char);
            continue;
        }

        if (execute_commands(context, command_start, command_end, command_line) != 0) {
            return 0;
        }

        repan_pattern_free(context->pattern);
        continue;
    }

    print_str(begin, data);
    return 0;
}

int main(int argc, char *argv[])
{
    test_context context;
    FILE *test_file;
    size_t size;
    char *data;
    int ret_value;

    if (argc <= 1) {
        printf("Usage: %s test_file_name\n\n", argv[0]);
        return 1;
    }

    test_file = fopen(argv[1], "r");

    if (test_file == NULL) {
        printf("Cannot open '%s'\n", argv[1]);
        return 1;
    }

    if (fseek(test_file, 0, SEEK_END)) {
        printf("Cannot read '%s'\n", argv[1]);
        fclose(test_file);
        return 1;
    }

    size = ftell(test_file);

    if (fseek(test_file, 0, SEEK_SET)) {
        printf("Cannot read '%s'\n", argv[1]);
        fclose(test_file);
        return 1;
    }

    data = (char*)malloc(size + 1);

    if (data == NULL) {
        printf("Cannot allocate memory");
        fclose(test_file);
        return 1;
    }

    if (fread(data, 1, size, test_file) != size) {
        printf("Cannot read '%s'\n", argv[1]);
        free(data);
        fclose(test_file);
        return 1;
    }

    fclose(test_file);

    data[size] = '\0';

    context.pattern_buffer_capacity = 1024;
    context.pattern_buffer = malloc(context.pattern_buffer_capacity);

    if (context.pattern_buffer == NULL) {
        printf("Error: Cannot allocate memory");
        free(data);
        return 1;
    }

    context.line = 1;
    ret_value = process_commands(&context, data, data + size);

    free(data);
    free(context.pattern_buffer);

    return ret_value;
}
