#!/usr/bin/env python

#    Regex Pattern Analyzer
#
#    Copyright Zoltan Herczeg (hzmester@freemail.hu). All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#   1. Redistributions of source code must retain the above copyright notice, this list of
#      conditions and the following disclaimer.
#
#   2. Redistributions in binary form must reproduce the above copyright notice, this list
#      of conditions and the following disclaimer in the documentation and/or other materials
#      provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
# SHALL THE COPYRIGHT HOLDER(S) OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# First download and extract https://www.unicode.org/Public/UCD/latest/ucd/UCD.zip into the same directory

import re

# Useful utility:
# https://unicode.org/cldr/utility/list-unicodeset.jsp

# L& for Lu Ll Lt (called LC by the standard)
# http://www.unicode.org/reports/tr44/#General_Category_Values
general_cathegories = [ "Lu", "Ll", "Lt", "Lm", "Lo", "Mn", "Mc", "Me", "Nd", "Nl", "No",
                        "Pc", "Pd", "Ps", "Pe", "Pi", "Pf", "Po", "Sm", "Sc", "Sk", "So",
                        "Zs", "Zl", "Zp", "Cc", "Cf", "Cs", "Co", "Cn" ]

script_names = {}
script_name_aliases = {}
property_list = {}
property_len_min = 1000
property_len_max = 0
max_codepoint_name_length = 127

# Adds a property name or alias
def add_property(name, description):
    global property_len_min
    global property_len_max

    length = len(name)

    if length < property_len_min:
        property_len_min = length
    if length > property_len_max:
        property_len_max = length

    property_group = property_list.get(length)
    if property_group == None:
        property_group = []
        property_list[length] = property_group

    property_group.append((name, description % (name)))

# ------------------------------------------------------------------------------
#   Parse UnicodeData.txt
# ------------------------------------------------------------------------------

codepoints = []
codepoint_name_block_size = 16
codepoint_name_data = []
codepoint_name_offsets = []

cathegories = {}
for idx, cathegory in enumerate(general_cathegories):
    cathegories[cathegory] = idx

def parse_unicode_data():
    codepoint_name_map = {}
    name_re = re.compile("^(?:[A-Z0-9]|-(?!-))+$")

    with open("UnicodeData.txt") as f:
        cathegory_re = re.compile("^([0-9A-F]{4,6});([^;]+);([^;]+);")
        last_codepoint = -1
        str_unknown = "Unknown"
        unknown_cp = (cathegories["Cn"], str_unknown, 0)
        in_block = None

        for line in f:
            match_obj = cathegory_re.match(line)
            if match_obj == None:
                raise Exception("Invalid record in UnicodeData.txt: %s" % (line))

            codepoint = int(match_obj.group(1), 16)
            if codepoint >= 0x110000:
                raise Exception("Codepoint too big: 0x%x" % (codepoint))
            if codepoint <= last_codepoint:
                raise Exception("Codepoint order error in UnicodeData.txt: %x -> %x" % (last_codepoint, codepoint))

            cathegory = cathegories.get(match_obj.group(3))
            if cathegory == None:
                raise Exception("Unknown cathegory in UnicodeData.txt: %s" % (match_obj.group(3)))

            if in_block:
                if in_block + "Last>" != match_obj.group(2):
                    raise Exception("Unexpected block terminator: '%s' instead of '%s'"
                                    % (match_obj.group(2), in_block + "Last>"))

                codepoint_tuple = codepoints[-1]

                if codepoint_tuple[0] != cathegory:
                    raise Exception("Block terminator cathegory mismatch: %s instead of %s"
                                    % (general_cathegories[cathegory], general_cathegories[codepoint_tuple[0]]))

                while last_codepoint < codepoint:
                     codepoints.append(codepoint_tuple)
                     last_codepoint += 1

                in_block = None;
                continue

            last_codepoint += 1
            while last_codepoint < codepoint:
                codepoints.append(unknown_cp)
                last_codepoint += 1

            name = match_obj.group(2)

            if name.endswith("First>"):
                in_block = match_obj.group(2)[0:-6]
            elif not name.startswith("<"):
                name = name.replace(" ", "")

                if name_re.match(name) == None:
                    raise Exception("Invalid character name: %s" % (name))

                if len(name) > max_codepoint_name_length:
                    raise Exception("Name limit reached (increase REPAN_U_MAX_NAME_LENGTH in literal.h): %s" % (name))

                name_no_hypen = name.replace("-", "")

                if name_no_hypen in codepoint_name_map:
                    codepoint_name_map[name_no_hypen].extend([name, codepoint])
                else:
                    codepoint_name_map[name_no_hypen] = [name, codepoint]

            codepoints.append((cathegory, str_unknown, 0))

    while len(codepoints) < 0x110000:
        codepoints.append(unknown_cp)

    with open("NameAliases.txt") as f:
        alias_re = re.compile("^([0-9A-F]{4,6});([^;]+);(?:correction|control|alternate|abbreviation)")

        for line in f:
            match_obj = alias_re.match(line)
            if match_obj == None:
                continue

            codepoint = int(match_obj.group(1), 16)
            name = match_obj.group(2).replace(" ", "")

            if name_re.match(name) == None:
                raise Exception("Invalid character name: %s" % (name))

            if len(name) > max_codepoint_name_length:
                raise Exception("Name limit reached (increase REPAN_U_MAX_NAME_LENGTH in literal.h): %s" % (name))

            name_no_hypen = name.replace("-", "")

            if name_no_hypen in codepoint_name_map:
                codepoint_name_map[name_no_hypen].extend([name, codepoint])
            else:
                codepoint_name_map[name_no_hypen] = [name, codepoint]

    codepoint_names = []
    for name, values in codepoint_name_map.items():
        if len(values) == 2:
            codepoint_names.append((name, values[1]))
            continue

        if len(values) > 4:
            raise Exception("More than two items with hypen differences are not supported")

        if len(values[0]) < len(values[2]):
            name1 = values[0]
            cp1 = values[1]
            name2 = values[2]
            cp2 = values[3]
        else:
            name1 = values[2]
            cp1 = values[3]
            name2 = values[0]
            cp2 = values[1]

        if name1.find("-") != -1 or len(name1) + 1 != len(name2):
            raise Exception("Multiple hypens are not supported (yet)")

        idx = name2.find("-")
        if idx == -1:
            raise Exception("Hypen is not present")

        codepoint_names.append((name2.replace("-", ":"), cp1))
        codepoint_names.append((name2.replace("-", "="), cp2))

    codepoint_names.sort(key=lambda tup: tup[0])

    idx = 0
    last = len(codepoint_names)
    ord_2 = ord("2")
    ord_9 = ord("9")
    ord_A = ord("A")
    ord_Z = ord("Z")

    if last % codepoint_name_block_size != 0:
        codepoint_names.append(("", 0))
        last += 1

    byte_buffer = [0, 0]

    # bit_length max is 8
    def push(value, bit_length):
        if bit_length > 8 or value >= (1 << bit_length):
            raise Exception("Internal error: push byte")

        byte_buffer[0] |= value << byte_buffer[1]
        byte_buffer[1] += bit_length
        if byte_buffer[1] >= 8:
            codepoint_name_data.append(byte_buffer[0] & 0xff)
            byte_buffer[0] >>= 8
            byte_buffer[1] -= 8

    while idx < last:
        name = codepoint_names[idx][0]
        prefix = 0

        if idx % codepoint_name_block_size == 0:
            if byte_buffer[1] > 0:
                codepoint_name_data.append(byte_buffer[0])
                byte_buffer = [0, 0]

            codepoint_name_offsets.append(len(codepoint_name_data))
        else:
            length = min(len(name), len(prev_name), 63)

            while prefix < length:
                if name[prefix] != prev_name[prefix]:
                    break
                prefix += 1

            push(prefix, 6)

        prev_name = name
        length = len(name)
        for ch_idx in range(prefix, length):
            ch = name[ch_idx]
            if ch == "0":
                push(26, 5)
                continue
            if ch == "1":
                push(27, 5)
                continue
            if ch == ":":
                push(29, 5)
                continue
            if ch == "=":
                push(30, 5)
                continue

            ch = ord(ch)
            if ch >= ord_2 and ch <= ord_9:
                push(28, 5)
                push(ch - ord_2, 3)
                continue

            if ch < ord_A or ch > ord_Z:
                raise Exception("Internal error: encode name")
            push(ch - ord_A, 5)

        push(31, 5)

        value = codepoint_names[idx][1]

        shift = 0
        while value >= 1 << (shift + 7):
            shift += 7

        while True:
            push((value >> shift) & 0x7f, 7)

            if shift == 0:
                push(0, 1)
                break

            push(1, 1)
            shift -= 7

        idx += 1

    if byte_buffer[1] > 0:
        codepoint_name_data.append(byte_buffer[0])

parse_unicode_data()

# ------------------------------------------------------------------------------
#   Parse Scripts.txt
# ------------------------------------------------------------------------------

def parse_scripts():
    with open("Scripts.txt") as f:
        script_re = re.compile("^([0-9A-F]{4,6})(?:\.\.([0-9A-F]{4,6}))? *; ([A-Za-z_]+) ")
        script_name_re = re.compile("^(?:[A-Za-z0-9]|_(?!_))+$")
        str_unknown = "Unknown"

        for line in f:
            match_obj = script_re.match(line)
            if match_obj == None:
                continue

            script = match_obj.group(3)
            if not script in script_names:
                if script_name_re.match(script) == None:
                    raise Exception("Invalid script name: %s" % (script))
                script_names[script] = []

            first = int(match_obj.group(1), 16)
            last = first
            if match_obj.group(2) != None:
                last = int(match_obj.group(2), 16)

            while first <= last:
                codepoint = codepoints[first]
                if codepoint[1] != str_unknown:
                    raise Exception("Codepoint script changed from: %s -> %s" % (codepoint[1], script))
                codepoints[first] = (codepoint[0], script, 0)
                first += 1

    with open("PropertyValueAliases.txt") as f:
        names_found = len(script_names)
        script_re = re.compile("^sc ; ([A-Za-z_]+) +; ([A-Za-z_]+)")

        for line in f:
            match_obj = script_re.match(line)
            if match_obj == None:
                continue

            alias = match_obj.group(1)
            name = match_obj.group(2)

            if not name in script_names:
                if name != "Katakana_Or_Hiragana" and name != "Unknown":
                    raise Exception("Unknown script name: %s" % (name))
                continue

            names_found -= 1

            if alias in script_name_aliases:
                raise Exception("Script alias is not unique: %s" % (alias))

            script_name_aliases[alias] = name

        if names_found != 0:
            raise Exception("Each script names must have an alias")

    with open("ScriptExtensions.txt") as f:
        script_re = re.compile("^([0-9A-F]{4,6})(?:\.\.([0-9A-F]{4,6}))? *;((?: +[A-Za-z_]+)+) ")

        for line in f:
            match_obj = script_re.match(line)
            if match_obj == None:
                continue

            first = int(match_obj.group(1), 16)
            last = first
            if match_obj.group(2) != None:
                last = int(match_obj.group(2), 16)

            for alias in match_obj.group(3).split(" "):
                if alias == "":
                    continue

                name = script_name_aliases.get(alias)
                if name == None:
                    raise Exception("Unknown script alias: %s" % (alias))

                char_list = script_names[name]

                for ch in range(first, last + 1):
                    if codepoints[ch][1] == name:
                        continue

                    char_list.append(ch)

    for name, char_list in script_names.items():
        char_list.sort()
        new_char_list = []

        start = -2
        end = -2
        for ch in char_list:
            if ch == end + 1:
                end = ch
                continue

            if start + 1 == end:
                new_char_list.append(start)
                new_char_list.append(end)
            elif start != end:
                if start == 0:
                    new_char_list.append(-0.5)
                else:
                    new_char_list.append(-start)
                new_char_list.append(end)
            elif start != -2:
                new_char_list.append(start)
            start = ch
            end = ch

        if start + 1 == end:
            new_char_list.append(start)
            new_char_list.append(end)
        elif start != end:
            if start == 0:
                new_char_list.append(-0.5)
            else:
                new_char_list.append(-start)
            new_char_list.append(end)
        elif start != -2:
            new_char_list.append(start)

        script_names[name] = new_char_list

parse_scripts()

# ------------------------------------------------------------------------------
#   Compute property names
# ------------------------------------------------------------------------------

property_char_data = []

def add_derived_property(name, include_cathegories):
    if name == "L&":
        uppercase_name = "L_AMPERSAND"
        add_property("L&", "     func(\"%s\", \"L&\", REPAN_UN_L_AMPERSAND, REPAN_U_DEFINE(L_AMPERSAND))")
    else:
        uppercase_name = name.upper()
        add_property(uppercase_name, "     func(\"%%s\", \"%s\", REPAN_UN_%s, REPAN_U_DEFINE(%s))"
                                     % (name, uppercase_name, uppercase_name))

    cathegory_flags = 0
    for cathegory in include_cathegories:
        cathegory_flags |= 1 << cathegories[cathegory]
    property_char_data.append((uppercase_name, cathegory_flags, None, None))

add_derived_property("Any", general_cathegories)
if general_cathegories[-1] != "Cn":
    raise Exception("Cn must be the last member of general cathegories.")
add_derived_property("Assigned", general_cathegories[0:-1])
add_derived_property("L&", ["Lu", "Ll", "Lt"])
add_property("LC", "     func(\"%s\", \"LC\", REPAN_UN_L_AMPERSAND, REPAN_U_DEFINE(L_AMPERSAND))")

def add_cathegories():
    last_cathegory = None
    cathegory_flags = 0

    for idx, cathegory in enumerate(general_cathegories):
        uppercase_cathegory = cathegory.upper()
        property_char_data.append((uppercase_cathegory, 1 << idx, None, None))
        add_property(uppercase_cathegory, "     func(\"%%s\", \"%s\", REPAN_UN_%s, REPAN_U_DEFINE(%s))"
                                          % (cathegory, uppercase_cathegory, uppercase_cathegory))

        current_cathegory = cathegory[0]
        if current_cathegory == last_cathegory:
           cathegory_flags |= 1 << idx
           continue

        if last_cathegory != None:
            property_char_data.append((last_cathegory, cathegory_flags, None, None))
            add_property(last_cathegory, "     func(\"%%s\", \"%s\", REPAN_UN_%s, REPAN_U_DEFINE(%s))"
                                         % (last_cathegory, last_cathegory, last_cathegory))
        last_cathegory = current_cathegory
        cathegory_flags = 1 << idx

    property_char_data.append((last_cathegory, cathegory_flags, None, None))
    add_property(last_cathegory, "     func(\"%%s\", \"%s\", REPAN_UN_%s, REPAN_U_DEFINE(%s))"
                                 % (last_cathegory, last_cathegory, last_cathegory))

    extra_cathegories = ["C", "L", "M", "N", "P", "S", "Z", "LC"]

    with open("PropertyValueAliases.txt") as f:
        names_found = len(script_names)
        cathegory_re = re.compile("^gc ; ([A-Za-z]+) +; ([A-Za-z_ ;]+)")

        for line in f:
            match_obj = cathegory_re.match(line)
            if match_obj == None:
                continue

            aliases = match_obj.group(2).replace(" ", "").split(";")
            cathegory = match_obj.group(1)

            if not cathegory in cathegories and not cathegory in extra_cathegories:
                raise Exception("Unknown cathegory: %s" % (cathegory))

            if cathegory == "LC":
                cathegory = "L_AMPERSAND"

            for alias in aliases:
                uppercase_alias = alias.upper()
                add_property(uppercase_alias.replace("_", ""), "     func(\"%%s\", \"%s\", REPAN_UN_%s, REPAN_U_DEFINE(%s))"
                                                               % (alias, uppercase_alias, cathegory.upper()))

add_cathegories()

def add_scripts():
    names = []
    for name in script_names:
        names.append((name, name))
    for alias, name in script_name_aliases.items():
        if alias != name:
            names.append((alias, name))
        elif not alias in script_names:
            raise Exception("Internal error: invalid alias")

    names.sort(key=lambda tup: tup[0])

    for name in names:
        name_upper = name[0].upper()
        add_property(name_upper.replace("_", ""), "     func(\"%%s\", \"%s\", REPAN_UN_%s, REPAN_U_DEFINE(%s))"
                                                  % (name[0], name_upper, name[1].upper()))

        if name[0] == name[1]:
            property_char_data.append((name_upper, 0, None, script_names.get(name[0])))

add_scripts()
script_name_aliases = None

# ------------------------------------------------------------------------------
#   Parse PropList.txt / DerivedCoreProperties.txt
# ------------------------------------------------------------------------------

cathegory_total = [0] * len(general_cathegories)
for codepoint in codepoints:
    cathegory_total[codepoint[0]] += 1;

def get_property_values(name, file_name):
    def list_append(value_list, start, end):
        if start == end:
            value_list.append(start)
        elif start + 1 == end:
            value_list.append(start)
            value_list.append(end)
        elif start != 0:
            value_list.append(-start)
            value_list.append(end)
        else:
            # Handling -0.0 is non trivial
            value_list.append(-0.5)
            value_list.append(end)

    derived_re = re.compile("^([0-9A-F]{4,6})(?:\.\.([0-9A-F]{4,6}))? *; %s " % (name))

    cathegory_data = [0] * len(general_cathegories)
    switches = []
    switch_start = -1
    switch_end = -1

    with open(file_name) as f:
        for line in f:
            match_obj = derived_re.match(line)
            if match_obj == None:
                continue

            codepoint = int(match_obj.group(1), 16)
            last = codepoint
            if match_obj.group(2) != None:
                last = int(match_obj.group(2), 16)

            if switch_end != codepoint:
                if switch_start >= 0:
                    switches.append(switch_start)
                    switches.append(switch_end)
                switch_start = codepoint
            switch_end = last + 1

            while codepoint <= last:
                cathegory_data[codepoints[codepoint][0]] += 1
                codepoint += 1

    if switch_start >= 0:
        switches.append(switch_start)
        switches.append(switch_end)

    print("Cathegory statistics of '%s'" % (name))
    sanity_include = 0
    sanity_exclude = 0
    cathegory_flags = 0
    for idx, value in enumerate(cathegory_data):
        if value == 0:
            continue
        total = cathegory_total[idx]
        print("  %s: %s [from %s %.1f%%]" % (general_cathegories[idx], value, total, value * 100.0 / total))

        if value * 2 >= total:
            cathegory_flags |= 1 << idx
            sanity_exclude += total - value
        else:
            sanity_include += value

    cathegory_data = None
    include = []
    exclude = []
    include_start = -2
    include_end = -2
    exclude_start = -2
    exclude_end = -2

    switches.append(0x110000)
    is_include = False
    switch_index = 0

    for codepoint in range(0, 0x110000):
        if codepoint == switches[switch_index]:
            is_include = not is_include
            switch_index += 1

        if cathegory_flags & (1 << codepoints[codepoint][0]):
            if not is_include:
                if exclude_end + 1 != codepoint:
                    if exclude_start >= 0:
                        list_append(exclude, exclude_start, exclude_end)
                        sanity_exclude -= exclude_end - exclude_start + 1
                    exclude_start = codepoint
                exclude_end = codepoint
        elif is_include:
            if include_end + 1 != codepoint:
                if include_start >= 0:
                    list_append(include, include_start, include_end)
                    sanity_include -= include_end - include_start + 1
                include_start = codepoint
            include_end = codepoint

    if exclude_start >= 0:
        list_append(exclude, exclude_start, exclude_end)
        sanity_exclude -= exclude_end - exclude_start + 1
    if include_start >= 0:
        list_append(include, include_start, include_end)
        sanity_include -= include_end - include_start + 1

    if sanity_exclude != 0 or sanity_include != 0:
        raise Exception("Derived property sanity check failed (exclude: %d include: %d)"
                        % (sanity_exclude, sanity_include))

    property_char_data.append((name.upper(), cathegory_flags, exclude, include))

add_property("ASCII", "     func(\"%s\", \"ASCII\", REPAN_UN_ASCII, REPAN_U_DEFINE(ASCII))")
property_char_data.append(("ASCII", 0, [], [-0.5, 0x7f]))

get_property_values("ID_Start", "DerivedCoreProperties.txt")
get_property_values("ID_Continue", "DerivedCoreProperties.txt")

# ------------------------------------------------------------------------------
#   Simple case folding database
# ------------------------------------------------------------------------------

case_folding_data = [ 0 ]
case_folding_data_hex = 0

def get_case_folding_data():
    global case_folding_data_hex

    derived_re = re.compile("^([0-9A-F]{4,6}); [CS]; ([0-9A-F]{4,6})")

    folding_lists = []
    cp_map = {}

    with open("CaseFolding.txt") as f:
        for line in f:
            match_obj = derived_re.match(line)
            if match_obj == None:
                continue

            cp1 = int(match_obj.group(1), 16)
            cp2 = int(match_obj.group(2), 16)

            idx = cp_map.get(cp1)
            if idx != None:
                cp_map[cp2] = idx
                folding_lists[idx].append(cp2)
                continue

            idx = cp_map.get(cp2)
            if idx != None:
                cp_map[cp1] = idx
                folding_lists[idx].append(cp1)
                continue

            idx = len(folding_lists)
            cp_map[cp1] = idx
            cp_map[cp2] = idx

            folding_lists.append([cp1, cp2])

    cp_map = {}

    for cp_list in folding_lists:
        if len(cp_list) != 2:
            continue

        cp1 = cp_list[0]
        cp2 = cp_list[1]

        diff = cp2 - cp1
        sign = 0
        if diff < 0:
            diff = -diff
            sign = 1
        idx = cp_map.get(diff)

        if idx == None:
            idx = len(case_folding_data)
            case_folding_data.append(diff)
            cp_map[diff] = idx

        codepoint = codepoints[cp1]
        codepoints[cp1] = (codepoint[0], codepoint[1], (idx << 2) | (sign << 1))

        sign = 1 - sign

        codepoint = codepoints[cp2]
        codepoints[cp2] = (codepoint[0], codepoint[1], (idx << 2) | (sign << 1))

    case_folding_data_hex = len(case_folding_data)

    for cp_list in folding_lists:
        if len(cp_list) == 2:
            continue

        cp_list.sort()
        idx = len(case_folding_data)

        for cp in cp_list:
            case_folding_data.append(cp)
            codepoint = codepoints[cp]
            codepoints[cp] = (codepoint[0], codepoint[1], (idx << 1) | 1)
        case_folding_data.append(0)

get_case_folding_data()

# ------------------------------------------------------------------------------
#   Create unicode_gen.h
# ------------------------------------------------------------------------------

def license(f):
    f.write("/*\n")
    f.write(" *    Regex Pattern Analyzer\n")
    f.write(" *\n")
    f.write(" *    Copyright Zoltan Herczeg (hzmester@freemail.hu). All rights reserved.\n")
    f.write(" *\n")
    f.write(" * Redistribution and use in source and binary forms, with or without modification, are\n")
    f.write(" * permitted provided that the following conditions are met:\n")
    f.write(" *\n")
    f.write(" *   1. Redistributions of source code must retain the above copyright notice, this list of\n")
    f.write(" *      conditions and the following disclaimer.\n")
    f.write(" *\n")
    f.write(" *   2. Redistributions in binary form must reproduce the above copyright notice, this list\n")
    f.write(" *      of conditions and the following disclaimer in the documentation and/or other materials\n")
    f.write(" *      provided with the distribution.\n")
    f.write(" *\n")
    f.write(" * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND CONTRIBUTORS ``AS IS'' AND ANY\n")
    f.write(" * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES\n")
    f.write(" * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT\n")
    f.write(" * SHALL THE COPYRIGHT HOLDER(S) OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,\n")
    f.write(" * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED\n")
    f.write(" * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR\n")
    f.write(" * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN\n")
    f.write(" * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN\n")
    f.write(" * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n")
    f.write(" */\n\n")
    f.write("/* WARNING: This file was automatically generated, do not edit! */\n\n")

with open("unicode_gen_inl.h", "w") as f:
    license(f)

    f.write("enum {")
    first = True
    for cathegory in general_cathegories:
        if not first:
            f.write(",")
        first = False
        f.write("\n    REPAN_UC_%s" % (cathegory))

    f.write("\n};\n\nenum {")
    offset = 0
    first = True
    for char_data in property_char_data:
        if not first:
            f.write(",")
        first = False
        f.write("\n    REPAN_UP_%s = %d" % (char_data[0], offset))

        offset += 1
        if char_data[2] != None:
            offset += 1 + len(char_data[2])
        if char_data[3] != None:
            offset += 1 + len(char_data[3])
    f.write("\n};\n\n")

    print("Size of REPAN_PRIV(u_property_data): %s bytes" % (offset * 4))

    f.write("#define REPAN_U_NAME_BLOCK_SIZE %d\n" % (codepoint_name_block_size))
    f.write("#define REPAN_U_NAME_OFFSETS_SIZE %d\n\n" % (len(codepoint_name_offsets)))

    f.write("#define REPAN_U_PROPERTIES(func)")

    prev = ""
    for i in range(property_len_min, property_len_max + 1):
        values = property_list.get(i)
        if values == None:
            continue

        values.sort(key=lambda tup: tup[0])
        for value in values:
            if prev == value[0]:
                raise Exception("Duplicated name: %s" % (prev))
            f.write(" \\\n" + value[1])
            prev = value[0]

    f.write("\n")

# ------------------------------------------------------------------------------
#   Create unicode_gen.c
# ------------------------------------------------------------------------------

def dump_stage(description, f, stage, step):
    known_items = {}
    next_id = 0
    column = 1000       # Special starting value.
    next_stage = []

    for i in range(0, len(stage), step):
        items = tuple(stage[i:i+step])
        items_id = known_items.get(items)
        if items_id != None:
            next_stage.append(items_id)
            continue

        for value in items:
            if column != 1000:
                f.write(",")
                column += 1
            if column >= 100:
                f.write("\n   ")
                column = 3
            string = " %d" % (value)
            f.write(string)
            column += len(string)

        next_stage.append(next_id)
        known_items[items] = next_id
        next_id += 1

    print("%s statistics: %d items, %d bytes"
          % (description, len(known_items), len(known_items) * step * 2))
    return next_stage

def dump_prop_list(f, prop_list, column):
    length = len(prop_list)
    string = " %d" % (length)

    column += len(string)
    f.write(string)

    for value in prop_list:
        f.write(",")
        column += 1

        if column >= 100:
            f.write("\n   ")
            column = 3

        if value == -0.5:
            string = " R$ | 0x0"
        elif value < 0:
            string = " R$ | 0x%x" % (-value)
        else:
            string = " 0x%x" % (value)

        column += len(string)
        f.write(string)

with open("unicode_gen.c", "w") as f:
    license(f)

    f.write("#include \"internal.h\"\n\n")
    known = {}
    next_id = 0

    f.write("enum {")

    names = ["UNKNOWN"]
    for name in script_names:
        names.append(name.upper())
    names.sort()

    first = True
    for name in names:
        if not first:
            f.write(",")
        first = False
        f.write("\n    REPAN_US_%s" % (name))
    names = None

    f.write("\n};\n\nstatic const repan_u_codepoint codepoints[] = {")
    first = True
    for i in range(0, 0x110000):
        codepoint = codepoints[i]
        codepoint_id = known.get(codepoint)

        if codepoint_id == None:
            if not first:
                f.write(",")
            first = False

            f.write("\n    { REPAN_UC_%s, REPAN_US_%s, %d }"
                    % (general_cathegories[codepoint[0]], codepoint[1].upper(), codepoint[2]))
            codepoint_id = next_id
            known[codepoint] = codepoint_id
            next_id += 1

        codepoints[i] = codepoint_id

    stage3_shift = 4
    f.write("\n};\n\nstatic const uint16_t stage3_map[] = {")
    stage2 = dump_stage("stage3", f, codepoints, 1 << stage3_shift)
    codepoints = None

    stage2_shift = 5
    f.write("\n};\n\nstatic const uint16_t stage2_map[] = {")
    stage1 = dump_stage("stage2", f, stage2, 1 << stage2_shift)
    stage2 = None

    f.write("\n};\n\nstatic const uint8_t stage1_map[] = {")

    column = 100
    first = True
    for value in stage1:
        if not first:
            f.write(",")
            column += 1
        first = False
        if column >= 100:
            f.write("\n   ")
            column = 3
        string = " %d" % (value)
        f.write(string)
        column += len(string)

    print("stage1 statistics: %d items, %d bytes" % (len(stage1), len(stage1) * 2))

    f.write("\n};\n\nconst uint8_t REPAN_PRIV(u_name_data)[] = {")

    column = 100
    first = True
    for value in codepoint_name_data:
        if not first:
            f.write(",")
            column += 1
        first = False
        if column >= 100:
            f.write("\n   ")
            column = 3
        string = " %d" % (value)
        f.write(string)
        column += len(string)

    f.write("\n};\n\nconst uint32_t REPAN_PRIV(u_name_offsets)[] = {")

    column = 100
    first = True
    for value in codepoint_name_offsets:
        if not first:
            f.write(",")
            column += 1
        first = False
        if column >= 100:
            f.write("\n   ")
            column = 3
        string = " %d" % (value)
        f.write(string)
        column += len(string)

    print("Unicode name data: %d bytes, %d offset bytes" % (len(codepoint_name_data), len(codepoint_name_offsets) * 4))

    f.write("\n};\n\nconst uint32_t REPAN_PRIV(u_case_folding)[] = {\n    /* Relative distances. */")

    column = 100
    first = True
    for idx, value in enumerate(case_folding_data):
        if not first:
            f.write(",")
            column += 1
        first = False

        if idx == case_folding_data_hex:
            f.write("\n    /* Case folding sets. */\n   ")
            column = 3
        elif column >= 100:
            f.write("\n   ")
            column = 3

        if idx < case_folding_data_hex:
            string = " %d" % (value)
        else:
            string = " 0x%x" % (value)
        f.write(string)
        column += len(string)

    f.write("\n};\n\n#define R$ REPAN_RANGE_START\n\n")
    f.write("/* Description of u_property_list can be found in \"literal.h\". */\n")
    f.write("const uint32_t REPAN_PRIV(u_property_list)[] = {\n")

    first = True

    for char_data in property_char_data:
         if not first:
             f.write(",\n")
         first = False

         if char_data[2] != None:
             f.write("    /* %s */ 0x%08x,\n    /* (-) */" % (char_data[0], char_data[1]))
             dump_prop_list(f, char_data[2], 13)
         elif char_data[3] != None:
             f.write("    REPAN_U_DEFINE_SCRIPT | REPAN_US_%s" % (char_data[0]))
         else:
             f.write("    /* %s */ REPAN_U_DEFINE_CATHEGORY | 0x%08x" % (char_data[0], char_data[1]));
             continue
         f.write(",\n    /* (+) */")
         dump_prop_list(f, char_data[3], 13)

    f.write("\n};\n\n#undef R$\n\n")

    f.write("const repan_u_codepoint *REPAN_PRIV(u_get_codepoint)(uint32_t chr)\n")
    f.write("{\n")
    f.write("    uint32_t offset = ((uint32_t)stage1_map[chr >> %d]) << %d;\n" % (stage3_shift + stage2_shift, stage2_shift))
    f.write("    offset = stage2_map[offset + ((chr >> %d) & 0x%x)] << %d;\n" % (stage3_shift, (1 << stage2_shift) - 1, stage3_shift))
    f.write("    offset = stage3_map[offset + (chr & 0x%x)];\n" % ((1 << stage3_shift) - 1))
    f.write("    return codepoints + offset;\n")
    f.write("}\n")
