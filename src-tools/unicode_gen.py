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

# L& for Lu Ll Lt (called LC by the standard)
# http://www.unicode.org/reports/tr44/#General_Category_Values
general_cathegories = [ "Lu", "Ll", "Lt", "Lm", "Lo", "Mn", "Mc", "Me", "Nd", "Nl", "No",
                        "Pc", "Pd", "Ps", "Pe", "Pi", "Pf", "Po", "Sm", "Sc", "Sk", "So",
                        "Zs", "Zl", "Zp", "Cc", "Cf", "Cs", "Co", "Cn" ]

script_names = {}
property_list = {}
property_len_min = 1000
property_len_max = 0

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

    property_group.append((name, description))

# ------------------------------------------------------------------------------
#   Parse UnicodeData.txt
# ------------------------------------------------------------------------------

codepoints = []

cathegories = { }
for idx, cathegory in enumerate(general_cathegories):
    cathegories[cathegory] = idx

cathegory_re = re.compile("^([0-9A-F]{4,6});[^;]+;([^;]+);")

last_codepoint = -1
str_unknown = "Unknown"
idx_cn = cathegories["Cn"]

with open("UnicodeData.txt") as f:
    for line in f:
        match_obj = cathegory_re.match(line)
        if match_obj == None:
            raise Exception("Invalid record in UnicodeData.txt: %s" % (line))

        codepoint = int(match_obj.group(1), 16)
        if codepoint >= 0x110000:
            raise Exception("Codepoint too big: 0x%x" % (codepoint))
        if codepoint <= last_codepoint:
            raise Exception("Codepoint order error in UnicodeData.txt: %x -> %x" % (last_codepoint, codepoint))

        last_codepoint += 1
        while last_codepoint < codepoint:
            codepoints.append((idx_cn, str_unknown))
            last_codepoint += 1

        cathegory = cathegories.get(match_obj.group(2))
        if cathegory == None:
            raise Exception("Unknown cathegory in UnicodeData.txt: %s" % (match_obj.group(2)))

        codepoints.append((cathegory, str_unknown))

while len(codepoints) < 0x110000:
    codepoints.append((idx_cn, str_unknown))


# ------------------------------------------------------------------------------
#   Parse Scripts.txt
# ------------------------------------------------------------------------------

script_re = re.compile("^([0-9A-F]{4,6})(?:\.\.([0-9A-F]{4,6}))? *; ([A-Za-z_]+) ")

with open("Scripts.txt") as f:
    for line in f:
        match_obj = script_re.match(line)
        if match_obj == None:
            continue

        script = script_names.get(match_obj.group(3))
        if script == None:
            script = match_obj.group(3)
            script_names[script] = script

        first = int(match_obj.group(1), 16)
        last = first
        if match_obj.group(2) != None:
            last = int(match_obj.group(2), 16)

        while first <= last:
            codepoint = codepoints[first]
            if codepoint[1] != str_unknown:
                raise Exception("Codepoint script changed from: %s -> %s" % (codepoint[1], script))
            codepoints[first] = (codepoint[0], script)
            first += 1

# ------------------------------------------------------------------------------
#   Compute names
# ------------------------------------------------------------------------------

property_char_data = []

def add_derived_property(name, uppercase_name, include_cathegories):
    add_property(name, "     func(\"%s\", REPAN_UC_NAME_%s, REPAN_U_DEFINE_PROPERTY(%s))"
                       % (name, uppercase_name, uppercase_name))
    cathegory_flags = 0
    for cathegory in include_cathegories:
        cathegory_flags |= cathegories[cathegory]
    property_char_data.append((uppercase_name, cathegory_flags, [], []))

add_derived_property("Any", "ANY", general_cathegories)
add_derived_property("L&", "L_AMPERSAND", ["Lu", "Ll", "Lt"])

for cathegory in general_cathegories:
    add_property(cathegory, "     func(\"%s\", REPAN_UC_NAME_%s, REPAN_U_DEFINE_CATHEGORY(%s))"
                            % (cathegory, cathegory, cathegory))

last_cathegory = None
cathegory_flags = 0

for idx, cathegory in enumerate(general_cathegories):
    current_cathegory = cathegory[0]
    if current_cathegory == last_cathegory:
       cathegory_flags |= 1 << idx
       continue

    if last_cathegory != None:
        property_char_data.append((last_cathegory, cathegory_flags, [], []))
        add_property(last_cathegory, "     func(\"%s\", REPAN_UC_NAME_%s, REPAN_U_DEFINE_PROPERTY(%s))"
                                     % (last_cathegory, last_cathegory, last_cathegory))
    last_cathegory = current_cathegory
    cathegory_flags = 1 << idx

property_char_data.append((last_cathegory, cathegory_flags, [], []))
add_property(last_cathegory, "     func(\"%s\", REPAN_UC_NAME_%s, REPAN_U_DEFINE_PROPERTY(%s))"
                             % (last_cathegory, last_cathegory, last_cathegory))

for name in script_names:
    name_upper = name.upper()
    add_property(name, "     func(\"%s\", REPAN_US_%s, REPAN_U_DEFINE_SCRIPT(%s))"
                       % (name, name_upper, name_upper))

# ------------------------------------------------------------------------------
#   Parse PropList.txt / DerivedCoreProperties.txt
# ------------------------------------------------------------------------------

cathegory_total = [0] * len(general_cathegories)
for codepoint in codepoints:
    cathegory_total[codepoint[0]] += 1;

def get_property_values(name, file_name):
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
                        exclude.append(exclude_start)
                        exclude.append(exclude_end)
                        sanity_exclude -= exclude_end - exclude_start + 1
                    exclude_start = codepoint
                exclude_end = codepoint
        elif is_include:
            if include_end + 1 != codepoint:
                if include_start >= 0:
                    include.append(include_start)
                    include.append(include_end)
                    sanity_include -= include_end - include_start + 1
                include_start = codepoint
            include_end = codepoint

    if exclude_start >= 0:
        exclude.append(exclude_start)
        exclude.append(exclude_end)
        sanity_exclude -= exclude_end - exclude_start + 1
    if include_start >= 0:
        include.append(include_start)
        include.append(include_end)
        sanity_include -= include_end - include_start + 1

    if sanity_exclude != 0 or sanity_include != 0:
        raise Exception("Derived property sanity check failed (exclude: %d include: %d)"
                        % (sanity_exclude, sanity_include))

    property_char_data.append((name.upper(), cathegory_flags, exclude, include))

get_property_values("ID_Start", "DerivedCoreProperties.txt")
get_property_values("ID_Continue", "DerivedCoreProperties.txt")

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

    f.write("enum {\n")
    for cathegory in general_cathegories:
        f.write("    REPAN_UC_%s,\n" % (cathegory))

    f.write("};\n\nenum {\n")
    offset = 0
    for char_data in property_char_data:
        f.write("    REPAN_UP_%s = %d,\n" % (char_data[0], offset))

        offset += 3
        length = len(char_data[2])
        if length > 0:
            offset += length + 1
        length = len(char_data[3])
        if length > 0:
            offset += length + 1
    f.write("};\n\n")

    print("Size of REPAN_PRIV(u_property_data): %s bytes" % (offset * 4))

    f.write("#define REPAN_U_PROPERTIES(func)")

    for i in range(property_len_min, property_len_max + 1):
        values = property_list.get(i)
        if values == None:
            continue

        values.sort(key=lambda tup: tup[0])
        for value in values:
            f.write(" \\\n" + value[1])

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
    if length == 0:
        f.write(" 1")
        return

    first = True
    for idx in range(-1, length + 1):
        if column >= 100:
            f.write(",\n   ")
            column = 3
        elif not first:
            f.write(",")
            column += 1
        first = False

        if idx == -1:
            string = " %d" % (length + 2)
        elif idx == length:
            string = " 0x110000"
        else:
            string = " 0x%x" % (prop_list[idx])

        column += len(string)
        f.write(string)

with open("unicode_gen.c", "w") as f:
    license(f)

    f.write("#include \"internal.h\"\n\n")
    known = {}
    next_id = 0

    f.write("static const repan_u_codepoint codepoints[] = {")
    first = True
    for i in range(0, 0x110000):
        codepoint = codepoints[i]
        codepoint_id = known.get(codepoint)

        if codepoint_id == None:
            if not first:
                f.write(",")
            first = False

            f.write("\n    { REPAN_UC_%s, REPAN_US_%s }"
                    % (general_cathegories[codepoint[0]], codepoint[1].upper()))
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

    f.write("\n};\n\n/* Description of u_property_map can be found in \"literal.h\". */\n")
    f.write("const uint32_t REPAN_PRIV(u_property_map)[] = {\n")

    first = True

    for char_data in property_char_data:
         if not first:
             f.write(",\n")
         first = False

         f.write("    /* %s */ 0x%08x,\n    /* (-) */" % (char_data[0], char_data[1]))
         dump_prop_list(f, char_data[2], 13)
         f.write(",\n    /* (+) */")
         dump_prop_list(f, char_data[3], 13)

    f.write("\n};\n\n")

    f.write("const repan_u_codepoint *REPAN_PRIV(u_get_codepoint)(uint32_t chr)\n")
    f.write("{\n")
    f.write("    uint32_t offset = ((uint32_t)stage1_map[chr >> %d]) << %d;\n" % (stage3_shift + stage2_shift, stage2_shift))
    f.write("    offset = stage2_map[offset + ((chr >> %d) & 0x%x)] << %d;\n" % (stage3_shift, (1 << stage2_shift) - 1, stage3_shift))
    f.write("    offset = stage3_map[offset + (chr & 0x%x)];\n" % ((1 << stage3_shift) - 1))
    f.write("    return codepoints + offset;\n")
    f.write("}\n")
