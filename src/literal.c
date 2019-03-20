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

static int is_ctype(const uint32_t *ctype_list, uint32_t chr)
{
    uint32_t size = *ctype_list;
    uint32_t left, right, value;

    if (size == 0) {
        return REPAN_FALSE;
    }

    ctype_list++;
    size--;

    left = ctype_list[0] & ~REPAN_RANGE_START;
    right = ctype_list[size];

    if (chr <= left) {
        return chr == left;
    }

    REPAN_ASSERT((right & REPAN_RANGE_START) == 0);

    if (chr >= right) {
        return chr == right;
    }

    REPAN_ASSERT(size >= 1);

    left = 0;
    right = size;

    while (REPAN_TRUE) {
        uint32_t mid = (left + right) >> 1;
        uint32_t chr_value;

        REPAN_ASSERT(left < right);

        value = ctype_list[mid];
        chr_value = value & ~REPAN_RANGE_START;

        if (chr < chr_value) {
            right = mid;
            continue;
        }

        if (chr == chr_value) {
            return REPAN_TRUE;
        }

        if (chr >= (ctype_list[mid + 1] & ~REPAN_RANGE_START)) {
            left = mid + 1;
            continue;
        }

        return (value & REPAN_RANGE_START) != 0;
    }
}

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

    return is_ctype(pattern->character_types[REPAN_CTYPE_WORD], chr);
}

int REPAN_PRIV(is_space)(repan_pattern *pattern, uint32_t chr)
{
    return is_ctype(pattern->character_types[REPAN_CTYPE_SPACE], chr);
}

int REPAN_PRIV(is_newline)(repan_pattern *pattern, uint32_t chr)
{
    if (pattern->options & REPAN_PARSE_UTF) {
        return (chr >= 0x0a && chr <= 0x0d) || chr == 0x85 || (chr | 0x1) == 0x2029;
    }

    return is_ctype(pattern->character_types[REPAN_CTYPE_NEWLINE], chr);
}

const uint32_t *REPAN_PRIV(get_other_cases)(repan_pattern *pattern, uint32_t chr, uint32_t *tmp_buf)
{
    uint16_t case_folding_offset;
    uint32_t other_case_offset;

    if (!(pattern->options & REPAN_PARSE_UTF)) {
        uint8_t other_case;

        if (chr >= pattern->char_tables_max) {
            return NULL;
        }

        other_case = pattern->case_folding[chr];
        if (other_case == chr) {
            return NULL;
        }

        if (chr < other_case) {
            tmp_buf[0] = chr;
            tmp_buf[1] = other_case;
        }
        else {
            tmp_buf[0] = other_case;
            tmp_buf[1] = chr;
        }

        return tmp_buf;
    }

    case_folding_offset = REPAN_PRIV(u_get_codepoint)(chr)->case_folding_offset;

    if (case_folding_offset == 0) {
        return NULL;
    }

    if (case_folding_offset & 0x1) {
        return REPAN_PRIV(u_case_folding) + (case_folding_offset >> 1);
    }

    other_case_offset = REPAN_PRIV(u_case_folding)[case_folding_offset >> 2];

    if (case_folding_offset & 0x2) {
        tmp_buf[0] = chr - other_case_offset;
        tmp_buf[1] = chr;
    }
    else {
        tmp_buf[0] = chr;
        tmp_buf[1] = chr + other_case_offset;
    }

    tmp_buf[2] = 0;
    return tmp_buf;
}

static uint32_t find_list_item(const repan_string_list_item *items, size_t number_of_items,
    uint32_t *string_start, size_t string_length)
{
    size_t min = 0;
    size_t max = number_of_items >> 1;
    uint32_t *src2_end = string_start + string_length;

    if (string_length < (items[0].data >> REPAN_STRING_LIST_LENGTH_SHIFT)
            || string_length > (items[max - 1].data >> REPAN_STRING_LIST_LENGTH_SHIFT))
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
                return (number_of_items & 0x1) ? middle + 1 : current_data;
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

#undef REPAN_GET_LIST_ITEM_DATA

uint32_t REPAN_PRIV(find_keyword)(uint32_t *keyword, size_t length)
{
    size_t number_of_items = sizeof(REPAN_PRIV(keywords)) * 2 / sizeof(repan_string_list_item);

    return find_list_item(REPAN_PRIV(keywords), number_of_items, keyword, length);
}

#define REPAN_GET_LIST_ITEM_DATA(name, type, data) \
    { (data | (uint32_t)((sizeof(name) - 1) << REPAN_STRING_LIST_LENGTH_SHIFT)), name },

const repan_string_list_item REPAN_PRIV(posix_classes)[] = {
    REPAN_POSIX_CLASSES(REPAN_GET_LIST_ITEM_DATA)
};

#undef REPAN_GET_LIST_ITEM_DATA

uint32_t REPAN_PRIV(find_posix_class)(uint32_t *name, size_t length)
{
    size_t number_of_items = sizeof(REPAN_PRIV(posix_classes)) * 2 / sizeof(repan_string_list_item);

    return find_list_item(REPAN_PRIV(posix_classes), number_of_items, name, length);
}

#define REPAN_GET_LIST_ITEM_DATA(name, real_name, type, data) \
    { (data | (uint32_t)((sizeof(name) - 1) << REPAN_STRING_LIST_LENGTH_SHIFT)), name },

#define REPAN_U_DEFINE(name) \
    (REPAN_UN_ ## name | (REPAN_UP_ ## name << REPAN_U_PROPERTY_TYPE_SHIFT))

const repan_string_list_item REPAN_PRIV(u_properties)[] = {
    REPAN_U_PROPERTIES(REPAN_GET_LIST_ITEM_DATA)
};

#undef REPAN_GET_LIST_ITEM_DATA
#undef REPAN_U_DEFINE

#define REPAN_GET_LIST_ITEM_DATA(name, real_name, type, data) \
    { real_name },

const repan_u_property_name_item REPAN_PRIV(u_property_names)[] = {
    REPAN_U_PROPERTIES(REPAN_GET_LIST_ITEM_DATA)
};

#undef REPAN_GET_LIST_ITEM_DATA

uint32_t REPAN_PRIV(find_u_property)(uint32_t *name, size_t length, int get_index)
{
    size_t number_of_items = sizeof(REPAN_PRIV(u_properties)) * 2 / sizeof(repan_string_list_item);

    if (get_index) {
        number_of_items |= 0x1;
    }

    return find_list_item(REPAN_PRIV(u_properties), number_of_items, name, length);
}

int REPAN_PRIV(u_match_property)(uint32_t chr, uint32_t property)
{
    uint32_t cathegory_flag = 1u << (uint32_t)(REPAN_PRIV(u_get_codepoint)(chr)->cathegory);
    const uint32_t *property_map = REPAN_PRIV(u_property_list) + property + 1;
    uint32_t size, left, right, value;

    if ((property_map[-1] & cathegory_flag) == 0) {
        property_map += property_map[0] + 1;
        return is_ctype(property_map, chr);
    }

    size = *property_map;
    if (size == 0) {
        return REPAN_TRUE;
    }

    property_map++;
    size--;

    left = property_map[0] & ~REPAN_RANGE_START;
    right = property_map[size];

    if (chr <= left) {
        return chr != left;
    }

    REPAN_ASSERT((right & REPAN_RANGE_START) == 0);

    if (chr >= right) {
        return chr != right;
    }

    REPAN_ASSERT(size >= 1);

    left = 0;
    right = size;

    while (REPAN_TRUE) {
        uint32_t mid = (left + right) >> 1;
        uint32_t chr_value;

        REPAN_ASSERT(left < right);

        value = property_map[mid];
        chr_value = value & ~REPAN_RANGE_START;

        if (chr < chr_value) {
            right = mid;
            continue;
        }

        if (chr == chr_value) {
            return REPAN_FALSE;
        }

        if (chr >= (property_map[mid + 1] & ~REPAN_RANGE_START)) {
            left = mid + 1;
            continue;
        }

        return (value & REPAN_RANGE_START) == 0;
    }
}

/*
  Result:
     -2: name is less than target_name
     -1: name is greater than target_name
      0: equal
*/
static int32_t u_compare_name(const uint8_t *name, const uint8_t *target_name)
{
    while (REPAN_TRUE) {
        if (*name == REPAN_CHAR_EQUALS_SIGN) {
            switch (*target_name) {
            case REPAN_CHAR_EQUALS_SIGN:
                target_name++;
                break;
            case REPAN_CHAR_COLON:
                return -1;
            }
            name++;
            continue;
        }
        else {
            switch (*target_name) {
            case REPAN_CHAR_EQUALS_SIGN:
                return -2;
            case REPAN_CHAR_COLON:
                target_name++;
                continue;
            }
        }

        if (*name != *target_name) {
            return -1 - (*name < *target_name);
        }
        if (*name == REPAN_CHAR_NUL) {
            return 0;
        }
        name++;
        target_name++;
    }
}

#define REPAN_READ_BITS(len) \
    REPAN_ASSERT(len > 0 && len <= 8); \
    value = bit_buffer & ((1 << len) - 1); \
    bit_buffer >>= len; \
    remaining_bits -= len; \
    if (remaining_bits < 8) { \
        bit_buffer |= (uint32_t)(*src++) << remaining_bits; \
        remaining_bits += 8; \
    }

static int32_t u_decode_block(const uint8_t *name, uint32_t block_index, int decode_count)
{
    uint8_t current_name[REPAN_U_MAX_NAME_LENGTH];
    const uint8_t *src = REPAN_PRIV(u_name_data) + REPAN_PRIV(u_name_offsets)[block_index];
    uint32_t bit_buffer = *src++;
    uint32_t remaining_bits = 8;
    int first = REPAN_TRUE;
    uint8_t *dst;

    do {
        uint32_t value, codepoint;
        int result;

        dst = current_name;

        if (!first) {
            REPAN_READ_BITS(6);
            dst += value;
        }
        first = REPAN_FALSE;

        while (REPAN_TRUE) {
            REPAN_READ_BITS(5);
            if (value < 26) {
                *dst++ = (uint8_t)(value + REPAN_CHAR_A);
                continue;
            }
            switch (value) {
            case 26:
                *dst++ = REPAN_CHAR_0;
                continue;
            case 27:
                *dst++ = REPAN_CHAR_1;
                continue;
            case 28:
                REPAN_READ_BITS(3);
                *dst++ = (uint8_t)(value + REPAN_CHAR_2);
                continue;
            case 29:
                *dst++ = REPAN_CHAR_COLON;
                continue;
            case 30:
                *dst++ = REPAN_CHAR_EQUALS_SIGN;
                continue;
            }
            break;
        }

        if (dst == current_name) {
            return -2;
        }

        *dst = REPAN_CHAR_NUL;

        codepoint = 0;
        do {
            REPAN_READ_BITS(7);
            codepoint = (codepoint << 7) | value;

            REPAN_READ_BITS(1);
        } while (value != 0);

        result = u_compare_name(name, current_name);
        if (result == 0) {
            return codepoint;
        }
        if (result == -2) {
            return -2;
        }
    } while (--decode_count > 0);

    return -1;
}

uint32_t REPAN_PRIV(u_find_name)(const uint8_t *name)
{
    uint32_t left = 0;
    uint32_t right = REPAN_U_NAME_OFFSETS_SIZE;
    int32_t result;

    while (REPAN_TRUE) {
        uint32_t mid = (left + right) >> 1;
        int decode_count = (left + 1 >= right) ? REPAN_U_NAME_BLOCK_SIZE : 1;

        REPAN_ASSERT(left < right);

        result = u_decode_block(name, mid, decode_count);

        REPAN_ASSERT(result >= -2 && result < 0x110000);

        if (result >= 0) {
            return result;
        }

        if (decode_count != 1) {
            return UINT32_MAX;
        }

        REPAN_ASSERT(left + 2 <= right);

        if (result == -2) {
            right = mid;
        }
        else {
            left = mid;
        }
    }
}
