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

int REPAN_PRIV(is_word_char)(repan_pattern *pattern, uint32_t chr)
{
    if (chr == REPAN_CHAR_UNDERSCORE) {
        return REPAN_TRUE;
    }

    if (pattern->options & REPAN_PARSE_UTF) {
        const repan_u_codepoint *codepoint = REPAN_PRIV(u_get_codepoint)(chr);

        return (codepoint->cathegory >= REPAN_UC_Lu && codepoint->cathegory <= REPAN_UC_Lo)
             || codepoint->cathegory == REPAN_UC_Nd;
    }

    if (REPAN_IS_DECIMAL_DIGIT(chr)) {
        return REPAN_TRUE;
    }

    return ((chr | 0x20) >= REPAN_CHAR_a && (chr | 0x20) <= REPAN_CHAR_z);
}

int REPAN_PRIV(is_space)(repan_pattern *pattern, uint32_t chr)
{
    return (chr == ' ' || chr == '\t') || REPAN_PRIV(is_newline)(pattern, chr);
}

int REPAN_PRIV(is_newline)(repan_pattern *pattern, uint32_t chr)
{
    if ((chr >= 0x0a && chr <= 0x0d) || chr == 0x85) {
        return REPAN_TRUE;
    }

    if (pattern->options & REPAN_PARSE_UTF) {
        return (chr | 0x1) == 0x2029;
    }

    return REPAN_FALSE;
}

static uint32_t find_list_item(const repan_string_list_item *items, size_t number_of_items,
    uint32_t *string_start, size_t string_length)
{
    size_t min = 0;
    size_t max = number_of_items;
    uint32_t *src2_end = string_start + string_length;

    if (string_length < (items[0].data >> REPAN_STRING_LIST_LENGTH_SHIFT)
            || string_length > (items[number_of_items - 1].data >> REPAN_STRING_LIST_LENGTH_SHIFT))
    {
        return 0;
    }

    do {
        size_t middle = (min + max) >> 1;
        uint32_t current_data = items[middle].data;
        size_t current_length = current_data >> REPAN_STRING_LIST_LENGTH_SHIFT;
        const char *src1;
        uint32_t *src2;

        if (string_length > current_length) {
            min = middle + 1;
            continue;
        }

        if (string_length < current_length) {
            max = middle;
            continue;
        }

        src1 = items[middle].string;
        src2 = string_start;

        while (REPAN_TRUE) {
            uint32_t src1chr = (uint32_t)*src1;
            uint32_t src2chr = *src2;

            if (src1chr < src2chr) {
                min = middle + 1;
                break;
            }

            if (src1chr > src2chr) {
                max = middle;
                break;
            }

            src1++;
            src2++;

            if (src2 >= src2_end) {
                return current_data;
            }
        }
    } while (min < max);

    return 0;
}

#define REPAN_GET_LIST_ITEM_DATA(name, type, data) \
    { (data | (uint32_t)((sizeof(name) - 1) << REPAN_STRING_LIST_LENGTH_SHIFT)), name },

const repan_string_list_item REPAN_PRIV(keywords)[] = {
REPAN_KEYWORDS(REPAN_GET_LIST_ITEM_DATA)
};

uint32_t REPAN_PRIV(find_keyword)(uint32_t *keyword, size_t length)
{
    size_t number_of_items = sizeof(REPAN_PRIV(keywords)) / sizeof(repan_string_list_item);

    return find_list_item(REPAN_PRIV(keywords), number_of_items, keyword, length);
}

const repan_string_list_item REPAN_PRIV(posix_classes)[] = {
REPAN_POSIX_CLASSES(REPAN_GET_LIST_ITEM_DATA)
};

uint32_t REPAN_PRIV(find_posix_class)(uint32_t *name, size_t length)
{
    size_t number_of_items = sizeof(REPAN_PRIV(posix_classes)) / sizeof(repan_string_list_item);

    return find_list_item(REPAN_PRIV(posix_classes), number_of_items, name, length);
}

#define REPAN_U_DEFINE_SCRIPT(name) \
    (REPAN_US_ ## name | REPAN_U_SCRIPT)

#define REPAN_U_DEFINE_PROPERTY(name) \
    (REPAN_UC_NAME_ ## name | (REPAN_UP_ ## name << 8) | REPAN_U_PROPERTY)

#define REPAN_U_DEFINE_CATHEGORY(name) \
    (REPAN_UC_NAME_ ## name | (REPAN_UC_ ## name << 8) | REPAN_U_CATHEGORY)

const repan_string_list_item REPAN_PRIV(u_properties)[] = {
REPAN_U_PROPERTIES(REPAN_GET_LIST_ITEM_DATA)
};

uint32_t REPAN_PRIV(find_u_property)(uint32_t *name, size_t length)
{
    size_t number_of_items = sizeof(REPAN_PRIV(u_properties)) / sizeof(repan_string_list_item);

    return find_list_item(REPAN_PRIV(u_properties), number_of_items, name, length);
}

int REPAN_PRIV(u_match_property)(uint32_t chr, uint32_t property)
{
    uint32_t cathegory_flag = 1u << (uint32_t)(REPAN_PRIV(u_get_codepoint)(chr)->cathegory);
    const uint32_t *property_map = REPAN_PRIV(u_property_map) + property + 1;
    uint32_t left, right;

    if ((property_map[-1] & cathegory_flag) != 0) {
        right = property_map[0];

        if (right == 1) {
            return REPAN_TRUE;
        }

        left = 0;
        property_map++;
        right -= 2;

        do {
            uint32_t mid = ((left + right) >> 1) & ~0x1u;

            if (chr < property_map[mid]) {
                right = mid;
            }
            else if (chr >= property_map[mid + 2]) {
                left = mid + 2;
            }
            else {
                return chr > property_map[mid + 1];
            }
        } while (left < right);

        return REPAN_TRUE;
    }

    property_map += property_map[0];

    right = property_map[0];

    if (right == 1) {
        return REPAN_FALSE;
    }

    left = 0;
    property_map++;
    right -= 2;

    do {
        uint32_t mid = ((left + right) >> 1) & ~0x1u;

        if (chr < property_map[mid]) {
            right = mid;
        }
        else if (chr >= property_map[mid + 2]) {
            left = mid + 2;
        }
        else {
            return chr <= property_map[mid + 1];
        }
    } while (left < right);

    return REPAN_FALSE;
}
