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

const char *repan_get_error_message(uint32_t error)
{
    switch (error) {
    case REPAN_SUCCESS:
        return "Operation is completed successfully";
    case REPAN_ERR_NO_MEMORY:
        return "Not enough memory";
    case REPAN_ERR_DAMAGED_PATTERN:
        return "The pattern is not usable after an unsuccessful operation and must be freed";
    case REPAN_ERR_CAPTURE_LIMIT:
        return "Too many capturing brackets";
    case REPAN_ERR_LENGTH_LIMIT:
        return "String is too long";
    case REPAN_ERR_LIST_LIMIT:
        return "Too many strings present in the pattern";
    case REPAN_ERR_INVALID_UTF_CHAR:
        return "Invalid UTF character";
    case REPAN_ERR_INVALID_BRACKET:
        return "Invalid subpattern type";
    case REPAN_ERR_UNTERMINATED_BRACKET:
        return "Unterminated subpattern";
    case REPAN_ERR_UNTERMINATED_CHAR_CLASS:
        return "Unterminated [...] character class";
    case REPAN_ERR_UNEXPECTED_BRACKET_CLOSE:
        return "Unexpected closing parenthesis";
    case REPAN_ERR_NOTHING_TO_REPEAT:
        return "Nothing to repeat before a quantifier";
    case REPAN_ERR_TOO_BIG_QUANTIFIER:
        return "Too big quantifier in {} repeat";
    case REPAN_ERR_QUANTIFIER_OUT_OF_ORDER:
        return "Quantifiers are out of order in {} repeat";
    case REPAN_ERR_TOO_LARGE_CHARACTER:
        return "Character value is too large";
    case REPAN_ERR_RIGHT_BRACKET_EXPECTED:
        return "A right bracket is expected";
    case REPAN_ERR_RIGHT_BRACE_EXPECTED:
        return "A right brace (}) is expected";
    case REPAN_ERR_GREATER_THAN_SIGN_EXPECTED:
        return "A greater than sign (>) is expected";
    case REPAN_ERR_EQUALS_SIGN_EXPECTED:
        return "An equals sign (=) is expected";
    case REPAN_ERR_APOSTROPHE_EXPECTED:
        return "An apostrophe (') is expected";
    case REPAN_ERR_COLON_EXPECTED:
        return "A colon (:) is expected";
    case REPAN_ERR_BACKSLASH_EXPECTED:
        return "A backslash (\\) is expected";
    case REPAN_ERR_SIGNED_INTEGER_REQUIRED:
        return "A decimal signed or unsigned integer is required";
    case REPAN_ERR_UNSIGNED_INTEGER_REQUIRED:
        return "A decimal unsigned integer is required";
    case REPAN_ERR_SIGNED_ZERO_IS_NOT_ALLOWED:
        return "Decimal zero with sign (+0 or -0) is not allowed";
    case REPAN_ERR_HEXADECIMAL_NUMBER_REQUIRED:
        return "A hexadecimal number enclosed in braces is required";
    case REPAN_ERR_OCTAL_NUMBER_REQUIRED:
        return "An octal number enclosed in braces is required";
    case REPAN_ERR_CAPTURING_BRACKET_NOT_EXIST:
        return "Referenced capturing bracket is non-existing";
    case REPAN_ERR_NAME_EXPECTED:
        return "A valid capturing group name is expected";
    case REPAN_ERR_INVALID_NAME_CHAR:
        return "A character allowed in capturing group names is expected";
    case REPAN_ERR_ESCAPE_NOT_ALLOWED_IN_CLASS:
        return "Escaping ascii letters is not allowed in character class";
    case REPAN_ERR_INVALID_RANGE:
        return "Neither the start nor the end of a range can be a character class";
    case REPAN_ERR_RANGE_OUT_OF_ORDER:
        return "Range start and end are out of order";
    case REPAN_ERR_TOO_MANY_ALTERNATIVES:
        return "No more than two alternatives are allowed for a conditional block";
    case REPAN_ERR_DEFINE_HAS_ALTERNATIVES:
        return "Alternatives are not allowed for a define block";
    case REPAN_ERR_INVALID_COND_ASSERT:
        return "Invalid lookahead/lookbehind assertion in a conditional block";
    case REPAN_ERR_INVALID_K_SEQUENCE:
        return "Only left brace ({), less than sign (<) or apostrophe (') are allowed after \\k";
    case REPAN_ERR_INVALID_C_SEQUENCE:
        return "A printable ascii character is required after \\c";
    case REPAN_ERR_INVALID_P_SEQUENCE:
        return "A valid property letter or property name in {...} required after \\p or \\P";
    case REPAN_ERR_INVALID_U_SEQUENCE:
        return "Four hexadecimal digits are required after \\u";
    case REPAN_ERR_UNKNOWN_PROPERTY:
        return "Unknown \\p or \\P property";
    case REPAN_ERR_UNKNOWN_VERB:
        return "Unknown control verb";
    case REPAN_ERR_MARK_ARGUMENT_EXPECTED:
        return "Argument for (*MARK) control verb is expected";
    case REPAN_ERR_UNKNOWN_OPTION:
        return "Unknown option in (?...) option list";
    case REPAN_ERR_UNTERMINATED_OPTION_LIST:
        return "Unterminated option list";
    case REPAN_ERR_INVALID_HYPEN_IN_OPTION_LIST:
        return "Invalid hypen in option list";
    case REPAN_ERR_UNICODE_NAMES_NOT_SUPPORTED:
        return "Unicode codepoint names are not supported";
    case REPAN_ERR_UNTERMINATED_RAW_CHARS:
        return "Unterminated \\Q...\\E sequence";
    case REPAN_ERR_UNTERMINATED_COMMENT:
        return "Unterminated comment";
    case REPAN_ERR_POSIX_INVALID_QUANTIFIER:
        return "Invalid {} repeat block";
    case REPAN_ERR_GLOB_INVALID_ASTERISK:
        return "A ** wildchard must be at the start or end of a pttern, or surrounded by directory separators";
    default:
        return "Unknown error code";
    }
}
