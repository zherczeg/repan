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

#ifndef REPAN_LITERAL_H
#define REPAN_LITERAL_H

#define REPAN_CHAR_NUL '\0'
#define REPAN_CHAR_LEFT_BRACKET '('
#define REPAN_CHAR_RIGHT_BRACKET ')'
#define REPAN_CHAR_PIPE '|'

#define REPAN_CHAR_ASTERISK '*'
#define REPAN_CHAR_PLUS '+'
#define REPAN_CHAR_MINUS '-'
#define REPAN_CHAR_QUESTION_MARK '?'
#define REPAN_CHAR_LEFT_BRACE '{'
#define REPAN_CHAR_RIGHT_BRACE '}'
#define REPAN_CHAR_COMMA ','

#define REPAN_CHAR_LEFT_SQUARE_BRACKET '['
#define REPAN_CHAR_RIGHT_SQUARE_BRACKET ']'

#define REPAN_CHAR_CIRCUMFLEX_ACCENT '^'
#define REPAN_CHAR_DOLLAR '$'
#define REPAN_CHAR_DOT '.'

#define REPAN_CHAR_BACKSLASH '\\'
#define REPAN_CHAR_COLON ':'
#define REPAN_CHAR_EQUALS_SIGN '='
#define REPAN_CHAR_AMPERSAND '&'
#define REPAN_CHAR_EXCLAMATION_MARK '!'
#define REPAN_CHAR_LESS_THAN_SIGN '<'
#define REPAN_CHAR_GREATER_THAN_SIGN '>'
#define REPAN_CHAR_APOSTROPHE '\''

#define REPAN_CHAR_UNDERSCORE '_'

#define REPAN_CHAR_HASHMARK '#'
#define REPAN_CHAR_SPACE ' '

#define REPAN_ESC_a 0x07
#define REPAN_ESC_b 0x08
#define REPAN_ESC_t 0x09
#define REPAN_ESC_n 0x0a
#define REPAN_ESC_f 0x0c
#define REPAN_ESC_r 0x0d
#define REPAN_ESC_e 0x1b

#define REPAN_CHAR_0 '0'
#define REPAN_CHAR_1 '1'
#define REPAN_CHAR_2 '2'
#define REPAN_CHAR_3 '3'
#define REPAN_CHAR_4 '4'
#define REPAN_CHAR_5 '5'
#define REPAN_CHAR_6 '6'
#define REPAN_CHAR_7 '7'
#define REPAN_CHAR_8 '8'
#define REPAN_CHAR_9 '9'

#define REPAN_CHAR_A 'A'
#define REPAN_CHAR_B 'B'
#define REPAN_CHAR_C 'C'
#define REPAN_CHAR_D 'D'
#define REPAN_CHAR_E 'E'
#define REPAN_CHAR_F 'F'
#define REPAN_CHAR_G 'G'
#define REPAN_CHAR_H 'H'
#define REPAN_CHAR_I 'I'
#define REPAN_CHAR_J 'J'
#define REPAN_CHAR_K 'K'
#define REPAN_CHAR_L 'L'
#define REPAN_CHAR_M 'M'
#define REPAN_CHAR_N 'N'
#define REPAN_CHAR_O 'O'
#define REPAN_CHAR_P 'P'
#define REPAN_CHAR_Q 'Q'
#define REPAN_CHAR_R 'R'
#define REPAN_CHAR_S 'S'
#define REPAN_CHAR_T 'T'
#define REPAN_CHAR_U 'U'
#define REPAN_CHAR_V 'V'
#define REPAN_CHAR_W 'W'
#define REPAN_CHAR_X 'X'
#define REPAN_CHAR_Y 'Y'
#define REPAN_CHAR_Z 'Z'

#define REPAN_CHAR_a 'a'
#define REPAN_CHAR_b 'b'
#define REPAN_CHAR_c 'c'
#define REPAN_CHAR_d 'd'
#define REPAN_CHAR_e 'e'
#define REPAN_CHAR_f 'f'
#define REPAN_CHAR_g 'g'
#define REPAN_CHAR_h 'h'
#define REPAN_CHAR_i 'i'
#define REPAN_CHAR_j 'j'
#define REPAN_CHAR_k 'k'
#define REPAN_CHAR_l 'l'
#define REPAN_CHAR_m 'm'
#define REPAN_CHAR_n 'n'
#define REPAN_CHAR_o 'o'
#define REPAN_CHAR_p 'p'
#define REPAN_CHAR_q 'q'
#define REPAN_CHAR_r 'r'
#define REPAN_CHAR_s 's'
#define REPAN_CHAR_t 't'
#define REPAN_CHAR_u 'u'
#define REPAN_CHAR_v 'v'
#define REPAN_CHAR_w 'w'
#define REPAN_CHAR_x 'x'
#define REPAN_CHAR_y 'y'
#define REPAN_CHAR_z 'z'

#define REPAN_IS_DECIMAL_DIGIT(chr) \
    ((chr) >= REPAN_CHAR_0 && (chr) <= REPAN_CHAR_9)

/* Matches a-z, A-z and nothing else. */
#define REPAN_IS_CASELESS_LATIN(chr) \
    (((chr) | 0x20) >= REPAN_CHAR_a && ((chr) | 0x20) <= REPAN_CHAR_z)

/* Matches a-z and nothing else. */
#define REPAN_IS_LOWERCASE_LATIN(chr) \
    ((chr) >= REPAN_CHAR_a && (chr) <= REPAN_CHAR_z)

int REPAN_PRIV(is_word_char)(repan_pattern *pattern, uint32_t chr);
int REPAN_PRIV(is_space)(repan_pattern *pattern, uint32_t chr);
int REPAN_PRIV(is_newline)(repan_pattern *pattern, uint32_t chr);

typedef struct {
    /* Lower 24 bit: free to use, higher 8 bit: length.
       Since length cannot be 0, data cannot be 0 as well. */
    uint32_t data;
    const char *string;
} repan_string_list_item;

#define REPAN_STRING_LIST_LENGTH_SHIFT 24

/* Keyword related data. */
#define REPAN_KEYW_TYPE_MASK 0xff
#define REPAN_KEYW_OPT_HAS_ARGUMENT 0x100
#define REPAN_KEYW_OPT_INIT 0x200
#define REPAN_KEYW_OPT_BRACKET 0x400

#define REPAN_KEYW_OPT_INIT_HAS_ARGUMENT (REPAN_KEYW_OPT_INIT | REPAN_KEYW_OPT_HAS_ARGUMENT)

/* Order: size, alpha */
#define REPAN_KEYWORDS(func) \
     func("F", REPAN_KEYW_F, REPAN_KEYW_FAIL | REPAN_KEYW_OPT_HAS_ARGUMENT) \
     func("CR", REPAN_KEYW_CR, REPAN_KEYW_CR | REPAN_KEYW_OPT_INIT) \
     func("LF", REPAN_KEYW_LF, REPAN_KEYW_LF | REPAN_KEYW_OPT_INIT) \
     func("sr", REPAN_KEYW_SR, REPAN_KEYW_SCRIPT_RUN | REPAN_KEYW_OPT_BRACKET) \
     func("ANY", REPAN_KEYW_ANY, REPAN_KEYW_ANY | REPAN_KEYW_OPT_INIT) \
     func("NUL", REPAN_KEYW_NUL, REPAN_KEYW_NUL | REPAN_KEYW_OPT_INIT) \
     func("UCP", REPAN_KEYW_UCP, REPAN_KEYW_UCP | REPAN_KEYW_OPT_INIT) \
     func("UTF", REPAN_KEYW_UTF, REPAN_KEYW_UTF | REPAN_KEYW_OPT_INIT) \
     func("CRLF", REPAN_KEYW_CRLF, REPAN_KEYW_CRLF | REPAN_KEYW_OPT_INIT) \
     func("FAIL", REPAN_KEYW_FAIL, REPAN_KEYW_FAIL | REPAN_KEYW_OPT_HAS_ARGUMENT) \
     func("MARK", REPAN_KEYW_MARK, REPAN_KEYW_MARK | REPAN_KEYW_OPT_HAS_ARGUMENT) \
     func("SKIP", REPAN_KEYW_SKIP, REPAN_KEYW_SKIP | REPAN_KEYW_OPT_HAS_ARGUMENT) \
     func("THEN", REPAN_KEYW_THEN, REPAN_KEYW_THEN | REPAN_KEYW_OPT_HAS_ARGUMENT) \
     func("PRUNE", REPAN_KEYW_PRUNE, REPAN_KEYW_PRUNE | REPAN_KEYW_OPT_HAS_ARGUMENT) \
     func("ACCEPT", REPAN_KEYW_ACCEPT, REPAN_KEYW_ACCEPT | REPAN_KEYW_OPT_HAS_ARGUMENT) \
     func("COMMIT", REPAN_KEYW_COMMIT, REPAN_KEYW_COMMIT | REPAN_KEYW_OPT_HAS_ARGUMENT) \
     func("NO_JIT", REPAN_KEYW_NO_JIT, REPAN_KEYW_NO_JIT | REPAN_KEYW_OPT_INIT) \
     func("ANYCRLF", REPAN_KEYW_ANYCRLF, REPAN_KEYW_ANYCRLF | REPAN_KEYW_OPT_INIT) \
     func("NOTEMPTY", REPAN_KEYW_NOTEMPTY, REPAN_KEYW_NOTEMPTY | REPAN_KEYW_OPT_INIT) \
     func("LIMIT_HEAP", REPAN_KEYW_LIMIT_HEAP, REPAN_KEYW_LIMIT_HEAP | REPAN_KEYW_OPT_INIT_HAS_ARGUMENT) \
     func("script_run", REPAN_KEYW_SCRIPT_RUN, REPAN_KEYW_SCRIPT_RUN | REPAN_KEYW_OPT_BRACKET) \
     func("BSR_ANYCRLF", REPAN_KEYW_BSR_ANYCRLF, REPAN_KEYW_BSR_ANYCRLF | REPAN_KEYW_OPT_INIT) \
     func("BSR_UNICODE", REPAN_KEYW_BSR_UNICODE, REPAN_KEYW_BSR_UNICODE | REPAN_KEYW_OPT_INIT) \
     func("LIMIT_DEPTH", REPAN_KEYW_LIMIT_DEPTH, REPAN_KEYW_LIMIT_DEPTH | REPAN_KEYW_OPT_INIT_HAS_ARGUMENT) \
     func("LIMIT_MATCH", REPAN_KEYW_LIMIT_MATCH, REPAN_KEYW_LIMIT_MATCH | REPAN_KEYW_OPT_INIT_HAS_ARGUMENT) \
     func("NO_START_OPT", REPAN_KEYW_NO_START_OPT, REPAN_KEYW_NO_START_OPT | REPAN_KEYW_OPT_INIT) \
     func("NO_AUTO_POSSESS", REPAN_KEYW_NO_AUTO_POSSESS, REPAN_KEYW_NO_AUTO_POSSESS | REPAN_KEYW_OPT_INIT) \
     func("NOTEMPTY_ATSTART", REPAN_KEYW_NOTEMPTY_ATSTART, REPAN_KEYW_NOTEMPTY_ATSTART | REPAN_KEYW_OPT_INIT) \
     func("NO_DOTSTAR_ANCHOR", REPAN_KEYW_NO_DOTSTAR_ANCHOR, REPAN_KEYW_NO_DOTSTAR_ANCHOR | REPAN_KEYW_OPT_INIT) \

enum {
#define REPAN_KEYWORD_TYPE(name, type, data) type,
REPAN_KEYWORDS(REPAN_KEYWORD_TYPE)
#undef REPAN_KEYWORD_TYPE
};

extern const repan_string_list_item REPAN_PRIV(keywords)[];
uint32_t REPAN_PRIV(find_keyword)(uint32_t *keyword, size_t length);

/* Perl / posix class related data. */

enum {
    REPAN_DECIMAL_DIGIT_CLASS,
    REPAN_SPACE_CLASS,
    REPAN_HORIZONTAL_SPACE_CLASS,
    REPAN_VERTICAL_SPACE_CLASS,
    REPAN_WORD_CHAR_CLASS,

    /* Negated version of the classes can be constructed
       by adding REPAN_NEG_PERL_CLASS to the class type. */
    REPAN_NEG_PERL_CLASS,
};

#define REPAN_POSIX_TYPE_MASK 0xff
#define REPAN_POSIX_OPT_IS_PERL 0x100

/* Order: size, alpha */
#define REPAN_POSIX_CLASSES(func) \
     func("word", REPAN_POSIX_WORD, REPAN_POSIX_OPT_IS_PERL | REPAN_WORD_CHAR_CLASS) \
     func("alnum", REPAN_POSIX_ALNUM, REPAN_POSIX_ALNUM) \
     func("alpha", REPAN_POSIX_ALPHA, REPAN_POSIX_ALPHA) \
     func("ascii", REPAN_POSIX_ASCII, REPAN_POSIX_ASCII) \
     func("blank", REPAN_POSIX_BLANK, REPAN_POSIX_BLANK) \
     func("cntrl", REPAN_POSIX_CNTRL, REPAN_POSIX_CNTRL) \
     func("digit", REPAN_POSIX_DIGIT, REPAN_POSIX_OPT_IS_PERL | REPAN_DECIMAL_DIGIT_CLASS) \
     func("graph", REPAN_POSIX_GRAPH, REPAN_POSIX_GRAPH) \
     func("lower", REPAN_POSIX_LOWER, REPAN_POSIX_LOWER) \
     func("print", REPAN_POSIX_PRINT, REPAN_POSIX_PRINT) \
     func("punct", REPAN_POSIX_PUNCT, REPAN_POSIX_PUNCT) \
     func("space", REPAN_POSIX_SPACE, REPAN_POSIX_OPT_IS_PERL | REPAN_SPACE_CLASS) \
     func("upper", REPAN_POSIX_UPPER, REPAN_POSIX_UPPER) \
     func("xdigit", REPAN_POSIX_XDIGIT, REPAN_POSIX_XDIGIT) \

enum {
#define REPAN_POSIX_CLASS_TYPE(name, type, data) type,
REPAN_POSIX_CLASSES(REPAN_POSIX_CLASS_TYPE)
#undef REPAN_POSIX_CLASS_TYPE
REPAN_NEG_POSIX_CLASS
};

extern const repan_string_list_item REPAN_PRIV(posix_classes)[];
uint32_t REPAN_PRIV(find_posix_class)(uint32_t *name, size_t length);

#define REPAN_U_ID_START 0x80
#define REPAN_U_ID_CONTINUE 0x80
#define REPAN_U_CATHEGORY_MASK 0x3f
#define REPAN_U_SCRIPT_MASK 0x3f

typedef struct {
    uint8_t cathegory;
    uint8_t script;
} repan_u_codepoint;

#include "unicode_gen_inl.h"

/* Property description: 8 bit: type, 6 bit: cathegory type, 6 bit: cathegory length */
#define REPAN_U_PROPERTY 0x100000
#define REPAN_U_CATHEGORY 0x200000
#define REPAN_U_SCRIPT 0x400000

#define REPAN_U_PROPERTY_TYPE_MASK 0xff
#define REPAN_U_GET_PROPERTY(data) ((data >> 8) & 0xfff)

#define REPAN_US_UNKNOWN 0xff

enum {
#define REPAN_UNICODE_PROPERT_TYPE(name, type, data) type,
REPAN_U_PROPERTIES(REPAN_UNICODE_PROPERT_TYPE)
#undef REPAN_UNICODE_PROPERT_TYPE
REPAN_NEG_U_PROPERTY_CLASS
};

extern const repan_string_list_item REPAN_PRIV(u_properties)[];
uint32_t REPAN_PRIV(find_u_property)(uint32_t *name, size_t length);
const repan_u_codepoint *REPAN_PRIV(u_get_codepoint)(uint32_t chr);

/* The property map contains information about Unicode Properties.
   The start offset of each property is defined as a REPAN_UP_*
   constant, e.g. REPAN_UP_ID_START. The first uint32 value of each
   map is a general cathegory bitset (REPAN_UC_Lu is bit 0,
   REPAN_UC_Ll is  bit 1, ...) which shows whether the characters of
   a cathegory should be included or excluded. There are exceptions
   though, and the exceptions are described as ordered list of ranges.
   The first list is the exclude list: it contains those characters
   which should be excluded even though their general cathegory is
   included. The second list called exclude list is the opposite:
   it contains those characters which should be included even though
   their general cathegory is excluded.
*/
extern const uint32_t REPAN_PRIV(u_property_map)[];

int REPAN_PRIV(u_match_property)(uint32_t chr, uint32_t property);

#endif /* REPAN_LITERAL_H */
