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
# https://www.unicode.org/reports/tr44/
general_cathegories = [ "Lu", "Ll", "Lt", "Lm", "Lo", "Mn", "Mc", "Me", "Nd", "Nl", "No",
                        "Pc", "Pd", "Ps", "Pe", "Pi", "Pf", "Po", "Sm", "Sc", "Sk", "So",
                        "Zs", "Zl", "Zp", "Cc", "Cf", "Cs", "Co", "Cn" ]

script_names = {}

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

property_list = {}
property_len_min = 1000
property_len_max = 0

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

# ------------------------------------------------------------------------------
#   Parse UnicodeData.txt
# ------------------------------------------------------------------------------

codepoints = []

cathegories = { }
for cathegory in general_cathegories:
    cathegories[cathegory] = cathegory

cathegory_re = re.compile("^([0-9A-F]{4,6});[^;]+;([^;]+);")

last_codepoint = -1
str_unknown = "Unknown"
str_cn = cathegories["Cn"]

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
            codepoints.append((str_cn, str_unknown, 0))
            last_codepoint += 1

        cathegory = cathegories.get(match_obj.group(2))
        if cathegory == None:
            raise Exception("Unknown cathegory in UnicodeData.txt: %s" % (match_obj.group(2)))

        codepoints.append((cathegory, str_unknown, 0))

while len(codepoints) < 0x110000:
    codepoints.append((str_cn, str_unknown, 0))


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
            codepoints[first] = (codepoint[0], script, codepoint[2])
            first += 1

# ------------------------------------------------------------------------------
#   Parse DerivedCoreProperties.txt
# ------------------------------------------------------------------------------

derived_re = re.compile("^([0-9A-F]{4,6})(?:\.\.([0-9A-F]{4,6}))? *; (ID_Start|ID_Continue) ")

ID_Start = 0x1
ID_Continue = 0x2

with open("DerivedCoreProperties.txt") as f:
    for line in f:
        match_obj = derived_re.match(line)
        if match_obj == None:
            continue

        flag = ID_Start
        if match_obj.group(3) == "ID_Continue":
            flag = ID_Continue

        first = int(match_obj.group(1), 16)
        last = first
        if match_obj.group(2) != None:
            last = int(match_obj.group(2), 16)

        while first <= last:
            codepoint = codepoints[first]
            if codepoint[2] & flag:
                raise Exception("Codepoint flag was set before")

            codepoints[first] = (codepoint[0], codepoint[1], codepoint[2] | flag)
            first += 1

# ------------------------------------------------------------------------------
#   Compute names
# ------------------------------------------------------------------------------

add_property("Any", "     func(\"Any\", REPAN_UC_NAME_ANY, REPAN_U_DEFINE_CATHEGORY(ANY, %s, %d))"
                    % (general_cathegories[0], len(general_cathegories)))
add_property("L&", "     func(\"L&\", REPAN_UC_NAME_L_AMPERSAND, REPAN_U_DEFINE_CATHEGORY(L_AMPERSAND, Lu, 3))")

for cathegory in general_cathegories:
    add_property(cathegory, "     func(\"%s\", REPAN_UC_NAME_%s, REPAN_U_DEFINE_CATHEGORY(%s, %s, 1))"
                            % (cathegory, cathegory, cathegory, cathegory))

last_cathegory = None
last_cathegory_first = None
last_cathegory_length = 0

for cathegory in general_cathegories:
    current_cathegory = cathegory[0]
    if current_cathegory == last_cathegory:
       last_cathegory_length += 1
       continue
    if last_cathegory != None:
        add_property(last_cathegory, "     func(\"%s\", REPAN_UC_NAME_%s, REPAN_U_DEFINE_CATHEGORY(%s, %s, %d))"
                                     % (last_cathegory, last_cathegory, last_cathegory, last_cathegory_first, last_cathegory_length))
    last_cathegory = current_cathegory
    last_cathegory_first = cathegory
    last_cathegory_length = 1

add_property(last_cathegory, "     func(\"%s\", REPAN_UC_NAME_%s, REPAN_U_DEFINE_CATHEGORY(%s, %s, %d))"
                             % (last_cathegory, last_cathegory, last_cathegory, last_cathegory_first, last_cathegory_length))

for name in script_names:
    name_upper = name.upper()
    add_property(name, "     func(\"%s\", REPAN_US_%s, REPAN_U_DEFINE_SCRIPT(%s))"
                       % (name, name_upper, name_upper))

# ------------------------------------------------------------------------------
#   Create unicode_gen.h
# ------------------------------------------------------------------------------

with open("unicode_gen_inl.h", "w") as f:
    license(f)

    f.write("enum {\n")
    for cathegory in general_cathegories:
        f.write("    REPAN_UC_%s,\n" % (cathegory))
    f.write("};\n\n")

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

with open("unicode_gen.c", "w") as f:
    license(f)

    f.write("#include \"internal.h\"\n\n")
    f.write("static const repan_u_codepoint codepoints[] = {\n")
    known = {}
    next_id = 0

    for i in range(0, 0x110000):
        codepoint = codepoints[i]
        codepoint_id = known.get(codepoint)

        if codepoint_id == None:
            flag1 = ""
            flag2 = ""

            if codepoint[2] & ID_Start:
                flag1 = " | REPAN_U_ID_START"
            if codepoint[2] & ID_Continue:
                flag2 = " | REPAN_U_ID_CONTINUE"

            f.write("    { REPAN_UC_%s%s, REPAN_US_%s%s },\n"
                    % (codepoint[0], flag1, codepoint[1].upper(), flag2))
            codepoint_id = next_id
            known[codepoint] = codepoint_id
            next_id += 1

        codepoints[i] = codepoint_id

    stage3_shift = 4
    f.write("};\n\nstatic const uint16_t stage3_map[] = {")
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

    f.write("\n};\n\n")
    f.write("const repan_u_codepoint *REPAN_PRIV(u_get_codepoint)(uint32_t value)\n")
    f.write("{\n")
    f.write("    uint32_t offset = ((uint32_t)stage1_map[value >> %d]) << %d;\n" % (stage3_shift + stage2_shift, stage2_shift))
    f.write("    offset = stage2_map[offset + ((value >> %d) & 0x%x)] << %d;\n" % (stage3_shift, (1 << stage2_shift) - 1, stage3_shift))
    f.write("    offset = stage3_map[offset + (value & 0x%x)];\n" % ((1 << stage3_shift) - 1))
    f.write("    return codepoints + offset;\n")
    f.write("}\n")
