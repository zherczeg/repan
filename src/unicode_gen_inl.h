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

/* WARNING: This file was automatically generated, do not edit! */

typedef struct {
    const uint16_t *stage3;
    uint32_t chr;
} repan_u_codepoint_iterator;

enum {
    REPAN_UC_Lu,
    REPAN_UC_Ll,
    REPAN_UC_Lt,
    REPAN_UC_Lm,
    REPAN_UC_Lo,
    REPAN_UC_Mn,
    REPAN_UC_Mc,
    REPAN_UC_Me,
    REPAN_UC_Nd,
    REPAN_UC_Nl,
    REPAN_UC_No,
    REPAN_UC_Pc,
    REPAN_UC_Pd,
    REPAN_UC_Ps,
    REPAN_UC_Pe,
    REPAN_UC_Pi,
    REPAN_UC_Pf,
    REPAN_UC_Po,
    REPAN_UC_Sm,
    REPAN_UC_Sc,
    REPAN_UC_Sk,
    REPAN_UC_So,
    REPAN_UC_Zs,
    REPAN_UC_Zl,
    REPAN_UC_Zp,
    REPAN_UC_Cc,
    REPAN_UC_Cf,
    REPAN_UC_Cs,
    REPAN_UC_Co,
    REPAN_UC_Cn
};

enum {
    REPAN_UP_ANY = 0,
    REPAN_UP_ASSIGNED = 1,
    REPAN_UP_L_AMPERSAND = 2,
    REPAN_UP_LU = 3,
    REPAN_UP_LL = 4,
    REPAN_UP_LT = 5,
    REPAN_UP_LM = 6,
    REPAN_UP_LO = 7,
    REPAN_UP_MN = 8,
    REPAN_UP_L = 9,
    REPAN_UP_MC = 10,
    REPAN_UP_ME = 11,
    REPAN_UP_ND = 12,
    REPAN_UP_M = 13,
    REPAN_UP_NL = 14,
    REPAN_UP_NO = 15,
    REPAN_UP_PC = 16,
    REPAN_UP_N = 17,
    REPAN_UP_PD = 18,
    REPAN_UP_PS = 19,
    REPAN_UP_PE = 20,
    REPAN_UP_PI = 21,
    REPAN_UP_PF = 22,
    REPAN_UP_PO = 23,
    REPAN_UP_SM = 24,
    REPAN_UP_P = 25,
    REPAN_UP_SC = 26,
    REPAN_UP_SK = 27,
    REPAN_UP_SO = 28,
    REPAN_UP_ZS = 29,
    REPAN_UP_S = 30,
    REPAN_UP_ZL = 31,
    REPAN_UP_ZP = 32,
    REPAN_UP_CC = 33,
    REPAN_UP_Z = 34,
    REPAN_UP_CF = 35,
    REPAN_UP_CS = 36,
    REPAN_UP_CO = 37,
    REPAN_UP_CN = 38,
    REPAN_UP_C = 39,
    REPAN_UP_ADLAM = 40,
    REPAN_UP_AHOM = 43,
    REPAN_UP_ANATOLIAN_HIEROGLYPHS = 45,
    REPAN_UP_ARABIC = 47,
    REPAN_UP_ARMENIAN = 58,
    REPAN_UP_AVESTAN = 61,
    REPAN_UP_BALINESE = 63,
    REPAN_UP_BAMUM = 65,
    REPAN_UP_BASSA_VAH = 67,
    REPAN_UP_BATAK = 69,
    REPAN_UP_BENGALI = 71,
    REPAN_UP_BHAIKSUKI = 89,
    REPAN_UP_BOPOMOFO = 91,
    REPAN_UP_BRAHMI = 108,
    REPAN_UP_BRAILLE = 110,
    REPAN_UP_BUGINESE = 112,
    REPAN_UP_BUHID = 115,
    REPAN_UP_CANADIAN_ABORIGINAL = 119,
    REPAN_UP_CARIAN = 121,
    REPAN_UP_CAUCASIAN_ALBANIAN = 123,
    REPAN_UP_CHAKMA = 125,
    REPAN_UP_CHAM = 131,
    REPAN_UP_CHEROKEE = 133,
    REPAN_UP_COMMON = 135,
    REPAN_UP_COPTIC = 137,
    REPAN_UP_CUNEIFORM = 141,
    REPAN_UP_CYPRIOT = 143,
    REPAN_UP_CYRILLIC = 151,
    REPAN_UP_DESERET = 156,
    REPAN_UP_DEVANAGARI = 158,
    REPAN_UP_DOGRA = 171,
    REPAN_UP_DUPLOYAN = 177,
    REPAN_UP_EGYPTIAN_HIEROGLYPHS = 181,
    REPAN_UP_ELBASAN = 183,
    REPAN_UP_ELYMAIC = 185,
    REPAN_UP_ETHIOPIC = 187,
    REPAN_UP_GEORGIAN = 189,
    REPAN_UP_GLAGOLITIC = 193,
    REPAN_UP_GOTHIC = 199,
    REPAN_UP_GRANTHA = 201,
    REPAN_UP_GREEK = 221,
    REPAN_UP_GUJARATI = 227,
    REPAN_UP_GUNJALA_GONDI = 235,
    REPAN_UP_GURMUKHI = 239,
    REPAN_UP_HAN = 247,
    REPAN_UP_HANGUL = 287,
    REPAN_UP_HANIFI_ROHINGYA = 302,
    REPAN_UP_HANUNOO = 309,
    REPAN_UP_HATRAN = 313,
    REPAN_UP_HEBREW = 315,
    REPAN_UP_HIRAGANA = 317,
    REPAN_UP_IMPERIAL_ARAMAIC = 342,
    REPAN_UP_INHERITED = 344,
    REPAN_UP_INSCRIPTIONAL_PAHLAVI = 346,
    REPAN_UP_INSCRIPTIONAL_PARTHIAN = 348,
    REPAN_UP_JAVANESE = 350,
    REPAN_UP_KAITHI = 353,
    REPAN_UP_KANNADA = 359,
    REPAN_UP_KATAKANA = 372,
    REPAN_UP_KAYAH_LI = 397,
    REPAN_UP_KHAROSHTHI = 400,
    REPAN_UP_KHMER = 402,
    REPAN_UP_KHOJKI = 404,
    REPAN_UP_KHUDAWADI = 410,
    REPAN_UP_LAO = 416,
    REPAN_UP_LATIN = 418,
    REPAN_UP_LEPCHA = 430,
    REPAN_UP_LIMBU = 432,
    REPAN_UP_LINEAR_A = 435,
    REPAN_UP_LINEAR_B = 439,
    REPAN_UP_LISU = 447,
    REPAN_UP_LYCIAN = 449,
    REPAN_UP_LYDIAN = 451,
    REPAN_UP_MAHAJANI = 453,
    REPAN_UP_MAKASAR = 459,
    REPAN_UP_MALAYALAM = 461,
    REPAN_UP_MANDAIC = 470,
    REPAN_UP_MANICHAEAN = 473,
    REPAN_UP_MARCHEN = 476,
    REPAN_UP_MASARAM_GONDI = 478,
    REPAN_UP_MEDEFAIDRIN = 482,
    REPAN_UP_MEETEI_MAYEK = 484,
    REPAN_UP_MENDE_KIKAKUI = 486,
    REPAN_UP_MEROITIC_CURSIVE = 488,
    REPAN_UP_MEROITIC_HIEROGLYPHS = 490,
    REPAN_UP_MIAO = 492,
    REPAN_UP_MODI = 494,
    REPAN_UP_MONGOLIAN = 498,
    REPAN_UP_MRO = 504,
    REPAN_UP_MULTANI = 506,
    REPAN_UP_MYANMAR = 510,
    REPAN_UP_NABATAEAN = 513,
    REPAN_UP_NANDINAGARI = 515,
    REPAN_UP_NEW_TAI_LUE = 526,
    REPAN_UP_NEWA = 528,
    REPAN_UP_NKO = 530,
    REPAN_UP_NUSHU = 532,
    REPAN_UP_NYIAKENG_PUACHUE_HMONG = 534,
    REPAN_UP_OGHAM = 536,
    REPAN_UP_OL_CHIKI = 538,
    REPAN_UP_OLD_HUNGARIAN = 540,
    REPAN_UP_OLD_ITALIC = 542,
    REPAN_UP_OLD_NORTH_ARABIAN = 544,
    REPAN_UP_OLD_PERMIC = 546,
    REPAN_UP_OLD_PERSIAN = 549,
    REPAN_UP_OLD_SOGDIAN = 551,
    REPAN_UP_OLD_SOUTH_ARABIAN = 553,
    REPAN_UP_OLD_TURKIC = 555,
    REPAN_UP_ORIYA = 557,
    REPAN_UP_OSAGE = 565,
    REPAN_UP_OSMANYA = 567,
    REPAN_UP_PAHAWH_HMONG = 569,
    REPAN_UP_PALMYRENE = 571,
    REPAN_UP_PAU_CIN_HAU = 573,
    REPAN_UP_PHAGS_PA = 575,
    REPAN_UP_PHOENICIAN = 580,
    REPAN_UP_PSALTER_PAHLAVI = 582,
    REPAN_UP_REJANG = 585,
    REPAN_UP_RUNIC = 587,
    REPAN_UP_SAMARITAN = 589,
    REPAN_UP_SAURASHTRA = 591,
    REPAN_UP_SHARADA = 593,
    REPAN_UP_SHAVIAN = 601,
    REPAN_UP_SIDDHAM = 603,
    REPAN_UP_SIGNWRITING = 605,
    REPAN_UP_SINHALA = 607,
    REPAN_UP_SOGDIAN = 611,
    REPAN_UP_SORA_SOMPENG = 614,
    REPAN_UP_SOYOMBO = 616,
    REPAN_UP_SUNDANESE = 618,
    REPAN_UP_SYLOTI_NAGRI = 620,
    REPAN_UP_SYRIAC = 626,
    REPAN_UP_TAGALOG = 636,
    REPAN_UP_TAGBANWA = 640,
    REPAN_UP_TAI_LE = 644,
    REPAN_UP_TAI_THAM = 648,
    REPAN_UP_TAI_VIET = 650,
    REPAN_UP_TAKRI = 652,
    REPAN_UP_TAMIL = 658,
    REPAN_UP_TANGUT = 670,
    REPAN_UP_TELUGU = 672,
    REPAN_UP_THAANA = 680,
    REPAN_UP_THAI = 690,
    REPAN_UP_TIBETAN = 692,
    REPAN_UP_TIFINAGH = 694,
    REPAN_UP_TIRHUTA = 696,
    REPAN_UP_UGARITIC = 705,
    REPAN_UP_VAI = 707,
    REPAN_UP_WANCHO = 709,
    REPAN_UP_WARANG_CITI = 711,
    REPAN_UP_YI = 713,
    REPAN_UP_ZANABAZAR_SQUARE = 724,
    REPAN_UP_ASCII = 726,
    REPAN_UP_ID_START = 731,
    REPAN_UP_ID_CONTINUE = 741
};

#define REPAN_U_NAME_BLOCK_SIZE 16
#define REPAN_U_NAME_OFFSETS_SIZE 2076

#define REPAN_U_PROPERTIES(func) \
     func("C", "C", REPAN_UN_C, REPAN_U_DEFINE(C)) \
     func("L", "L", REPAN_UN_L, REPAN_U_DEFINE(L)) \
     func("M", "M", REPAN_UN_M, REPAN_U_DEFINE(M)) \
     func("N", "N", REPAN_UN_N, REPAN_U_DEFINE(N)) \
     func("P", "P", REPAN_UN_P, REPAN_U_DEFINE(P)) \
     func("S", "S", REPAN_UN_S, REPAN_U_DEFINE(S)) \
     func("Z", "Z", REPAN_UN_Z, REPAN_U_DEFINE(Z)) \
     func("CC", "Cc", REPAN_UN_CC, REPAN_U_DEFINE(CC)) \
     func("CF", "Cf", REPAN_UN_CF, REPAN_U_DEFINE(CF)) \
     func("CN", "Cn", REPAN_UN_CN, REPAN_U_DEFINE(CN)) \
     func("CO", "Co", REPAN_UN_CO, REPAN_U_DEFINE(CO)) \
     func("CS", "Cs", REPAN_UN_CS, REPAN_U_DEFINE(CS)) \
     func("L&", "L&", REPAN_UN_L_AMPERSAND, REPAN_U_DEFINE(L_AMPERSAND)) \
     func("LC", "LC", REPAN_UN_LC, REPAN_U_DEFINE(L_AMPERSAND)) \
     func("LL", "Ll", REPAN_UN_LL, REPAN_U_DEFINE(LL)) \
     func("LM", "Lm", REPAN_UN_LM, REPAN_U_DEFINE(LM)) \
     func("LO", "Lo", REPAN_UN_LO, REPAN_U_DEFINE(LO)) \
     func("LT", "Lt", REPAN_UN_LT, REPAN_U_DEFINE(LT)) \
     func("LU", "Lu", REPAN_UN_LU, REPAN_U_DEFINE(LU)) \
     func("MC", "Mc", REPAN_UN_MC, REPAN_U_DEFINE(MC)) \
     func("ME", "Me", REPAN_UN_ME, REPAN_U_DEFINE(ME)) \
     func("MN", "Mn", REPAN_UN_MN, REPAN_U_DEFINE(MN)) \
     func("ND", "Nd", REPAN_UN_ND, REPAN_U_DEFINE(ND)) \
     func("NL", "Nl", REPAN_UN_NL, REPAN_U_DEFINE(NL)) \
     func("NO", "No", REPAN_UN_NO, REPAN_U_DEFINE(NO)) \
     func("PC", "Pc", REPAN_UN_PC, REPAN_U_DEFINE(PC)) \
     func("PD", "Pd", REPAN_UN_PD, REPAN_U_DEFINE(PD)) \
     func("PE", "Pe", REPAN_UN_PE, REPAN_U_DEFINE(PE)) \
     func("PF", "Pf", REPAN_UN_PF, REPAN_U_DEFINE(PF)) \
     func("PI", "Pi", REPAN_UN_PI, REPAN_U_DEFINE(PI)) \
     func("PO", "Po", REPAN_UN_PO, REPAN_U_DEFINE(PO)) \
     func("PS", "Ps", REPAN_UN_PS, REPAN_U_DEFINE(PS)) \
     func("SC", "Sc", REPAN_UN_SC, REPAN_U_DEFINE(SC)) \
     func("SK", "Sk", REPAN_UN_SK, REPAN_U_DEFINE(SK)) \
     func("SM", "Sm", REPAN_UN_SM, REPAN_U_DEFINE(SM)) \
     func("SO", "So", REPAN_UN_SO, REPAN_U_DEFINE(SO)) \
     func("YI", "Yi", REPAN_UN_YI, REPAN_U_DEFINE(YI)) \
     func("ZL", "Zl", REPAN_UN_ZL, REPAN_U_DEFINE(ZL)) \
     func("ZP", "Zp", REPAN_UN_ZP, REPAN_U_DEFINE(ZP)) \
     func("ZS", "Zs", REPAN_UN_ZS, REPAN_U_DEFINE(ZS)) \
     func("ANY", "Any", REPAN_UN_ANY, REPAN_U_DEFINE(ANY)) \
     func("HAN", "Han", REPAN_UN_HAN, REPAN_U_DEFINE(HAN)) \
     func("LAO", "Lao", REPAN_UN_LAO, REPAN_U_DEFINE(LAO)) \
     func("MRO", "Mro", REPAN_UN_MRO, REPAN_U_DEFINE(MRO)) \
     func("NKO", "Nko", REPAN_UN_NKO, REPAN_U_DEFINE(NKO)) \
     func("VAI", "Vai", REPAN_UN_VAI, REPAN_U_DEFINE(VAI)) \
     func("ADLM", "Adlm", REPAN_UN_ADLM, REPAN_U_DEFINE(ADLAM)) \
     func("AGHB", "Aghb", REPAN_UN_AGHB, REPAN_U_DEFINE(CAUCASIAN_ALBANIAN)) \
     func("AHOM", "Ahom", REPAN_UN_AHOM, REPAN_U_DEFINE(AHOM)) \
     func("ARAB", "Arab", REPAN_UN_ARAB, REPAN_U_DEFINE(ARABIC)) \
     func("ARMI", "Armi", REPAN_UN_ARMI, REPAN_U_DEFINE(IMPERIAL_ARAMAIC)) \
     func("ARMN", "Armn", REPAN_UN_ARMN, REPAN_U_DEFINE(ARMENIAN)) \
     func("AVST", "Avst", REPAN_UN_AVST, REPAN_U_DEFINE(AVESTAN)) \
     func("BALI", "Bali", REPAN_UN_BALI, REPAN_U_DEFINE(BALINESE)) \
     func("BAMU", "Bamu", REPAN_UN_BAMU, REPAN_U_DEFINE(BAMUM)) \
     func("BASS", "Bass", REPAN_UN_BASS, REPAN_U_DEFINE(BASSA_VAH)) \
     func("BATK", "Batk", REPAN_UN_BATK, REPAN_U_DEFINE(BATAK)) \
     func("BENG", "Beng", REPAN_UN_BENG, REPAN_U_DEFINE(BENGALI)) \
     func("BHKS", "Bhks", REPAN_UN_BHKS, REPAN_U_DEFINE(BHAIKSUKI)) \
     func("BOPO", "Bopo", REPAN_UN_BOPO, REPAN_U_DEFINE(BOPOMOFO)) \
     func("BRAH", "Brah", REPAN_UN_BRAH, REPAN_U_DEFINE(BRAHMI)) \
     func("BRAI", "Brai", REPAN_UN_BRAI, REPAN_U_DEFINE(BRAILLE)) \
     func("BUGI", "Bugi", REPAN_UN_BUGI, REPAN_U_DEFINE(BUGINESE)) \
     func("BUHD", "Buhd", REPAN_UN_BUHD, REPAN_U_DEFINE(BUHID)) \
     func("CAKM", "Cakm", REPAN_UN_CAKM, REPAN_U_DEFINE(CHAKMA)) \
     func("CANS", "Cans", REPAN_UN_CANS, REPAN_U_DEFINE(CANADIAN_ABORIGINAL)) \
     func("CARI", "Cari", REPAN_UN_CARI, REPAN_U_DEFINE(CARIAN)) \
     func("CHAM", "Cham", REPAN_UN_CHAM, REPAN_U_DEFINE(CHAM)) \
     func("CHER", "Cher", REPAN_UN_CHER, REPAN_U_DEFINE(CHEROKEE)) \
     func("COPT", "Copt", REPAN_UN_COPT, REPAN_U_DEFINE(COPTIC)) \
     func("CPRT", "Cprt", REPAN_UN_CPRT, REPAN_U_DEFINE(CYPRIOT)) \
     func("CYRL", "Cyrl", REPAN_UN_CYRL, REPAN_U_DEFINE(CYRILLIC)) \
     func("DEVA", "Deva", REPAN_UN_DEVA, REPAN_U_DEFINE(DEVANAGARI)) \
     func("DOGR", "Dogr", REPAN_UN_DOGR, REPAN_U_DEFINE(DOGRA)) \
     func("DSRT", "Dsrt", REPAN_UN_DSRT, REPAN_U_DEFINE(DESERET)) \
     func("DUPL", "Dupl", REPAN_UN_DUPL, REPAN_U_DEFINE(DUPLOYAN)) \
     func("EGYP", "Egyp", REPAN_UN_EGYP, REPAN_U_DEFINE(EGYPTIAN_HIEROGLYPHS)) \
     func("ELBA", "Elba", REPAN_UN_ELBA, REPAN_U_DEFINE(ELBASAN)) \
     func("ELYM", "Elym", REPAN_UN_ELYM, REPAN_U_DEFINE(ELYMAIC)) \
     func("ETHI", "Ethi", REPAN_UN_ETHI, REPAN_U_DEFINE(ETHIOPIC)) \
     func("GEOR", "Geor", REPAN_UN_GEOR, REPAN_U_DEFINE(GEORGIAN)) \
     func("GLAG", "Glag", REPAN_UN_GLAG, REPAN_U_DEFINE(GLAGOLITIC)) \
     func("GONG", "Gong", REPAN_UN_GONG, REPAN_U_DEFINE(GUNJALA_GONDI)) \
     func("GONM", "Gonm", REPAN_UN_GONM, REPAN_U_DEFINE(MASARAM_GONDI)) \
     func("GOTH", "Goth", REPAN_UN_GOTH, REPAN_U_DEFINE(GOTHIC)) \
     func("GRAN", "Gran", REPAN_UN_GRAN, REPAN_U_DEFINE(GRANTHA)) \
     func("GREK", "Grek", REPAN_UN_GREK, REPAN_U_DEFINE(GREEK)) \
     func("GUJR", "Gujr", REPAN_UN_GUJR, REPAN_U_DEFINE(GUJARATI)) \
     func("GURU", "Guru", REPAN_UN_GURU, REPAN_U_DEFINE(GURMUKHI)) \
     func("HANG", "Hang", REPAN_UN_HANG, REPAN_U_DEFINE(HANGUL)) \
     func("HANI", "Hani", REPAN_UN_HANI, REPAN_U_DEFINE(HAN)) \
     func("HANO", "Hano", REPAN_UN_HANO, REPAN_U_DEFINE(HANUNOO)) \
     func("HATR", "Hatr", REPAN_UN_HATR, REPAN_U_DEFINE(HATRAN)) \
     func("HEBR", "Hebr", REPAN_UN_HEBR, REPAN_U_DEFINE(HEBREW)) \
     func("HIRA", "Hira", REPAN_UN_HIRA, REPAN_U_DEFINE(HIRAGANA)) \
     func("HLUW", "Hluw", REPAN_UN_HLUW, REPAN_U_DEFINE(ANATOLIAN_HIEROGLYPHS)) \
     func("HMNG", "Hmng", REPAN_UN_HMNG, REPAN_U_DEFINE(PAHAWH_HMONG)) \
     func("HMNP", "Hmnp", REPAN_UN_HMNP, REPAN_U_DEFINE(NYIAKENG_PUACHUE_HMONG)) \
     func("HUNG", "Hung", REPAN_UN_HUNG, REPAN_U_DEFINE(OLD_HUNGARIAN)) \
     func("ITAL", "Ital", REPAN_UN_ITAL, REPAN_U_DEFINE(OLD_ITALIC)) \
     func("JAVA", "Java", REPAN_UN_JAVA, REPAN_U_DEFINE(JAVANESE)) \
     func("KALI", "Kali", REPAN_UN_KALI, REPAN_U_DEFINE(KAYAH_LI)) \
     func("KANA", "Kana", REPAN_UN_KANA, REPAN_U_DEFINE(KATAKANA)) \
     func("KHAR", "Khar", REPAN_UN_KHAR, REPAN_U_DEFINE(KHAROSHTHI)) \
     func("KHMR", "Khmr", REPAN_UN_KHMR, REPAN_U_DEFINE(KHMER)) \
     func("KHOJ", "Khoj", REPAN_UN_KHOJ, REPAN_U_DEFINE(KHOJKI)) \
     func("KNDA", "Knda", REPAN_UN_KNDA, REPAN_U_DEFINE(KANNADA)) \
     func("KTHI", "Kthi", REPAN_UN_KTHI, REPAN_U_DEFINE(KAITHI)) \
     func("LANA", "Lana", REPAN_UN_LANA, REPAN_U_DEFINE(TAI_THAM)) \
     func("LAOO", "Laoo", REPAN_UN_LAOO, REPAN_U_DEFINE(LAO)) \
     func("LATN", "Latn", REPAN_UN_LATN, REPAN_U_DEFINE(LATIN)) \
     func("LEPC", "Lepc", REPAN_UN_LEPC, REPAN_U_DEFINE(LEPCHA)) \
     func("LIMB", "Limb", REPAN_UN_LIMB, REPAN_U_DEFINE(LIMBU)) \
     func("LINA", "Lina", REPAN_UN_LINA, REPAN_U_DEFINE(LINEAR_A)) \
     func("LINB", "Linb", REPAN_UN_LINB, REPAN_U_DEFINE(LINEAR_B)) \
     func("LISU", "Lisu", REPAN_UN_LISU, REPAN_U_DEFINE(LISU)) \
     func("LYCI", "Lyci", REPAN_UN_LYCI, REPAN_U_DEFINE(LYCIAN)) \
     func("LYDI", "Lydi", REPAN_UN_LYDI, REPAN_U_DEFINE(LYDIAN)) \
     func("MAHJ", "Mahj", REPAN_UN_MAHJ, REPAN_U_DEFINE(MAHAJANI)) \
     func("MAKA", "Maka", REPAN_UN_MAKA, REPAN_U_DEFINE(MAKASAR)) \
     func("MAND", "Mand", REPAN_UN_MAND, REPAN_U_DEFINE(MANDAIC)) \
     func("MANI", "Mani", REPAN_UN_MANI, REPAN_U_DEFINE(MANICHAEAN)) \
     func("MARC", "Marc", REPAN_UN_MARC, REPAN_U_DEFINE(MARCHEN)) \
     func("MARK", "Mark", REPAN_UN_MARK, REPAN_U_DEFINE(M)) \
     func("MEDF", "Medf", REPAN_UN_MEDF, REPAN_U_DEFINE(MEDEFAIDRIN)) \
     func("MEND", "Mend", REPAN_UN_MEND, REPAN_U_DEFINE(MENDE_KIKAKUI)) \
     func("MERC", "Merc", REPAN_UN_MERC, REPAN_U_DEFINE(MEROITIC_CURSIVE)) \
     func("MERO", "Mero", REPAN_UN_MERO, REPAN_U_DEFINE(MEROITIC_HIEROGLYPHS)) \
     func("MIAO", "Miao", REPAN_UN_MIAO, REPAN_U_DEFINE(MIAO)) \
     func("MLYM", "Mlym", REPAN_UN_MLYM, REPAN_U_DEFINE(MALAYALAM)) \
     func("MODI", "Modi", REPAN_UN_MODI, REPAN_U_DEFINE(MODI)) \
     func("MONG", "Mong", REPAN_UN_MONG, REPAN_U_DEFINE(MONGOLIAN)) \
     func("MROO", "Mroo", REPAN_UN_MROO, REPAN_U_DEFINE(MRO)) \
     func("MTEI", "Mtei", REPAN_UN_MTEI, REPAN_U_DEFINE(MEETEI_MAYEK)) \
     func("MULT", "Mult", REPAN_UN_MULT, REPAN_U_DEFINE(MULTANI)) \
     func("MYMR", "Mymr", REPAN_UN_MYMR, REPAN_U_DEFINE(MYANMAR)) \
     func("NAND", "Nand", REPAN_UN_NAND, REPAN_U_DEFINE(NANDINAGARI)) \
     func("NARB", "Narb", REPAN_UN_NARB, REPAN_U_DEFINE(OLD_NORTH_ARABIAN)) \
     func("NBAT", "Nbat", REPAN_UN_NBAT, REPAN_U_DEFINE(NABATAEAN)) \
     func("NEWA", "Newa", REPAN_UN_NEWA, REPAN_U_DEFINE(NEWA)) \
     func("NKOO", "Nkoo", REPAN_UN_NKOO, REPAN_U_DEFINE(NKO)) \
     func("NSHU", "Nshu", REPAN_UN_NSHU, REPAN_U_DEFINE(NUSHU)) \
     func("OGAM", "Ogam", REPAN_UN_OGAM, REPAN_U_DEFINE(OGHAM)) \
     func("OLCK", "Olck", REPAN_UN_OLCK, REPAN_U_DEFINE(OL_CHIKI)) \
     func("ORKH", "Orkh", REPAN_UN_ORKH, REPAN_U_DEFINE(OLD_TURKIC)) \
     func("ORYA", "Orya", REPAN_UN_ORYA, REPAN_U_DEFINE(ORIYA)) \
     func("OSGE", "Osge", REPAN_UN_OSGE, REPAN_U_DEFINE(OSAGE)) \
     func("OSMA", "Osma", REPAN_UN_OSMA, REPAN_U_DEFINE(OSMANYA)) \
     func("PALM", "Palm", REPAN_UN_PALM, REPAN_U_DEFINE(PALMYRENE)) \
     func("PAUC", "Pauc", REPAN_UN_PAUC, REPAN_U_DEFINE(PAU_CIN_HAU)) \
     func("PERM", "Perm", REPAN_UN_PERM, REPAN_U_DEFINE(OLD_PERMIC)) \
     func("PHAG", "Phag", REPAN_UN_PHAG, REPAN_U_DEFINE(PHAGS_PA)) \
     func("PHLI", "Phli", REPAN_UN_PHLI, REPAN_U_DEFINE(INSCRIPTIONAL_PAHLAVI)) \
     func("PHLP", "Phlp", REPAN_UN_PHLP, REPAN_U_DEFINE(PSALTER_PAHLAVI)) \
     func("PHNX", "Phnx", REPAN_UN_PHNX, REPAN_U_DEFINE(PHOENICIAN)) \
     func("PLRD", "Plrd", REPAN_UN_PLRD, REPAN_U_DEFINE(MIAO)) \
     func("PRTI", "Prti", REPAN_UN_PRTI, REPAN_U_DEFINE(INSCRIPTIONAL_PARTHIAN)) \
     func("RJNG", "Rjng", REPAN_UN_RJNG, REPAN_U_DEFINE(REJANG)) \
     func("ROHG", "Rohg", REPAN_UN_ROHG, REPAN_U_DEFINE(HANIFI_ROHINGYA)) \
     func("RUNR", "Runr", REPAN_UN_RUNR, REPAN_U_DEFINE(RUNIC)) \
     func("SAMR", "Samr", REPAN_UN_SAMR, REPAN_U_DEFINE(SAMARITAN)) \
     func("SARB", "Sarb", REPAN_UN_SARB, REPAN_U_DEFINE(OLD_SOUTH_ARABIAN)) \
     func("SAUR", "Saur", REPAN_UN_SAUR, REPAN_U_DEFINE(SAURASHTRA)) \
     func("SGNW", "Sgnw", REPAN_UN_SGNW, REPAN_U_DEFINE(SIGNWRITING)) \
     func("SHAW", "Shaw", REPAN_UN_SHAW, REPAN_U_DEFINE(SHAVIAN)) \
     func("SHRD", "Shrd", REPAN_UN_SHRD, REPAN_U_DEFINE(SHARADA)) \
     func("SIDD", "Sidd", REPAN_UN_SIDD, REPAN_U_DEFINE(SIDDHAM)) \
     func("SIND", "Sind", REPAN_UN_SIND, REPAN_U_DEFINE(KHUDAWADI)) \
     func("SINH", "Sinh", REPAN_UN_SINH, REPAN_U_DEFINE(SINHALA)) \
     func("SOGD", "Sogd", REPAN_UN_SOGD, REPAN_U_DEFINE(SOGDIAN)) \
     func("SOGO", "Sogo", REPAN_UN_SOGO, REPAN_U_DEFINE(OLD_SOGDIAN)) \
     func("SORA", "Sora", REPAN_UN_SORA, REPAN_U_DEFINE(SORA_SOMPENG)) \
     func("SOYO", "Soyo", REPAN_UN_SOYO, REPAN_U_DEFINE(SOYOMBO)) \
     func("SUND", "Sund", REPAN_UN_SUND, REPAN_U_DEFINE(SUNDANESE)) \
     func("SYLO", "Sylo", REPAN_UN_SYLO, REPAN_U_DEFINE(SYLOTI_NAGRI)) \
     func("SYRC", "Syrc", REPAN_UN_SYRC, REPAN_U_DEFINE(SYRIAC)) \
     func("TAGB", "Tagb", REPAN_UN_TAGB, REPAN_U_DEFINE(TAGBANWA)) \
     func("TAKR", "Takr", REPAN_UN_TAKR, REPAN_U_DEFINE(TAKRI)) \
     func("TALE", "Tale", REPAN_UN_TALE, REPAN_U_DEFINE(TAI_LE)) \
     func("TALU", "Talu", REPAN_UN_TALU, REPAN_U_DEFINE(NEW_TAI_LUE)) \
     func("TAML", "Taml", REPAN_UN_TAML, REPAN_U_DEFINE(TAMIL)) \
     func("TANG", "Tang", REPAN_UN_TANG, REPAN_U_DEFINE(TANGUT)) \
     func("TAVT", "Tavt", REPAN_UN_TAVT, REPAN_U_DEFINE(TAI_VIET)) \
     func("TELU", "Telu", REPAN_UN_TELU, REPAN_U_DEFINE(TELUGU)) \
     func("TFNG", "Tfng", REPAN_UN_TFNG, REPAN_U_DEFINE(TIFINAGH)) \
     func("TGLG", "Tglg", REPAN_UN_TGLG, REPAN_U_DEFINE(TAGALOG)) \
     func("THAA", "Thaa", REPAN_UN_THAA, REPAN_U_DEFINE(THAANA)) \
     func("THAI", "Thai", REPAN_UN_THAI, REPAN_U_DEFINE(THAI)) \
     func("TIBT", "Tibt", REPAN_UN_TIBT, REPAN_U_DEFINE(TIBETAN)) \
     func("TIRH", "Tirh", REPAN_UN_TIRH, REPAN_U_DEFINE(TIRHUTA)) \
     func("UGAR", "Ugar", REPAN_UN_UGAR, REPAN_U_DEFINE(UGARITIC)) \
     func("VAII", "Vaii", REPAN_UN_VAII, REPAN_U_DEFINE(VAI)) \
     func("WARA", "Wara", REPAN_UN_WARA, REPAN_U_DEFINE(WARANG_CITI)) \
     func("WCHO", "Wcho", REPAN_UN_WCHO, REPAN_U_DEFINE(WANCHO)) \
     func("XPEO", "Xpeo", REPAN_UN_XPEO, REPAN_U_DEFINE(OLD_PERSIAN)) \
     func("XSUX", "Xsux", REPAN_UN_XSUX, REPAN_U_DEFINE(CUNEIFORM)) \
     func("YIII", "Yiii", REPAN_UN_YIII, REPAN_U_DEFINE(YI)) \
     func("ZANB", "Zanb", REPAN_UN_ZANB, REPAN_U_DEFINE(ZANABAZAR_SQUARE)) \
     func("ZINH", "Zinh", REPAN_UN_ZINH, REPAN_U_DEFINE(INHERITED)) \
     func("ZYYY", "Zyyy", REPAN_UN_ZYYY, REPAN_U_DEFINE(COMMON)) \
     func("ADLAM", "Adlam", REPAN_UN_ADLAM, REPAN_U_DEFINE(ADLAM)) \
     func("ASCII", "ASCII", REPAN_UN_ASCII, REPAN_U_DEFINE(ASCII)) \
     func("BAMUM", "Bamum", REPAN_UN_BAMUM, REPAN_U_DEFINE(BAMUM)) \
     func("BATAK", "Batak", REPAN_UN_BATAK, REPAN_U_DEFINE(BATAK)) \
     func("BUHID", "Buhid", REPAN_UN_BUHID, REPAN_U_DEFINE(BUHID)) \
     func("CNTRL", "cntrl", REPAN_UN_CNTRL, REPAN_U_DEFINE(CC)) \
     func("DIGIT", "digit", REPAN_UN_DIGIT, REPAN_U_DEFINE(ND)) \
     func("DOGRA", "Dogra", REPAN_UN_DOGRA, REPAN_U_DEFINE(DOGRA)) \
     func("GREEK", "Greek", REPAN_UN_GREEK, REPAN_U_DEFINE(GREEK)) \
     func("KHMER", "Khmer", REPAN_UN_KHMER, REPAN_U_DEFINE(KHMER)) \
     func("LATIN", "Latin", REPAN_UN_LATIN, REPAN_U_DEFINE(LATIN)) \
     func("LIMBU", "Limbu", REPAN_UN_LIMBU, REPAN_U_DEFINE(LIMBU)) \
     func("NUSHU", "Nushu", REPAN_UN_NUSHU, REPAN_U_DEFINE(NUSHU)) \
     func("OGHAM", "Ogham", REPAN_UN_OGHAM, REPAN_U_DEFINE(OGHAM)) \
     func("ORIYA", "Oriya", REPAN_UN_ORIYA, REPAN_U_DEFINE(ORIYA)) \
     func("OSAGE", "Osage", REPAN_UN_OSAGE, REPAN_U_DEFINE(OSAGE)) \
     func("OTHER", "Other", REPAN_UN_OTHER, REPAN_U_DEFINE(C)) \
     func("PUNCT", "punct", REPAN_UN_PUNCT, REPAN_U_DEFINE(P)) \
     func("RUNIC", "Runic", REPAN_UN_RUNIC, REPAN_U_DEFINE(RUNIC)) \
     func("TAILE", "Tai_Le", REPAN_UN_TAI_LE, REPAN_U_DEFINE(TAI_LE)) \
     func("TAKRI", "Takri", REPAN_UN_TAKRI, REPAN_U_DEFINE(TAKRI)) \
     func("TAMIL", "Tamil", REPAN_UN_TAMIL, REPAN_U_DEFINE(TAMIL)) \
     func("ARABIC", "Arabic", REPAN_UN_ARABIC, REPAN_U_DEFINE(ARABIC)) \
     func("BRAHMI", "Brahmi", REPAN_UN_BRAHMI, REPAN_U_DEFINE(BRAHMI)) \
     func("CARIAN", "Carian", REPAN_UN_CARIAN, REPAN_U_DEFINE(CARIAN)) \
     func("CHAKMA", "Chakma", REPAN_UN_CHAKMA, REPAN_U_DEFINE(CHAKMA)) \
     func("COMMON", "Common", REPAN_UN_COMMON, REPAN_U_DEFINE(COMMON)) \
     func("COPTIC", "Coptic", REPAN_UN_COPTIC, REPAN_U_DEFINE(COPTIC)) \
     func("FORMAT", "Format", REPAN_UN_FORMAT, REPAN_U_DEFINE(CF)) \
     func("GOTHIC", "Gothic", REPAN_UN_GOTHIC, REPAN_U_DEFINE(GOTHIC)) \
     func("HANGUL", "Hangul", REPAN_UN_HANGUL, REPAN_U_DEFINE(HANGUL)) \
     func("HATRAN", "Hatran", REPAN_UN_HATRAN, REPAN_U_DEFINE(HATRAN)) \
     func("HEBREW", "Hebrew", REPAN_UN_HEBREW, REPAN_U_DEFINE(HEBREW)) \
     func("KAITHI", "Kaithi", REPAN_UN_KAITHI, REPAN_U_DEFINE(KAITHI)) \
     func("KHOJKI", "Khojki", REPAN_UN_KHOJKI, REPAN_U_DEFINE(KHOJKI)) \
     func("LEPCHA", "Lepcha", REPAN_UN_LEPCHA, REPAN_U_DEFINE(LEPCHA)) \
     func("LETTER", "Letter", REPAN_UN_LETTER, REPAN_U_DEFINE(L)) \
     func("LYCIAN", "Lycian", REPAN_UN_LYCIAN, REPAN_U_DEFINE(LYCIAN)) \
     func("LYDIAN", "Lydian", REPAN_UN_LYDIAN, REPAN_U_DEFINE(LYDIAN)) \
     func("NUMBER", "Number", REPAN_UN_NUMBER, REPAN_U_DEFINE(N)) \
     func("REJANG", "Rejang", REPAN_UN_REJANG, REPAN_U_DEFINE(REJANG)) \
     func("SYMBOL", "Symbol", REPAN_UN_SYMBOL, REPAN_U_DEFINE(S)) \
     func("SYRIAC", "Syriac", REPAN_UN_SYRIAC, REPAN_U_DEFINE(SYRIAC)) \
     func("TANGUT", "Tangut", REPAN_UN_TANGUT, REPAN_U_DEFINE(TANGUT)) \
     func("TELUGU", "Telugu", REPAN_UN_TELUGU, REPAN_U_DEFINE(TELUGU)) \
     func("THAANA", "Thaana", REPAN_UN_THAANA, REPAN_U_DEFINE(THAANA)) \
     func("WANCHO", "Wancho", REPAN_UN_WANCHO, REPAN_U_DEFINE(WANCHO)) \
     func("AVESTAN", "Avestan", REPAN_UN_AVESTAN, REPAN_U_DEFINE(AVESTAN)) \
     func("BENGALI", "Bengali", REPAN_UN_BENGALI, REPAN_U_DEFINE(BENGALI)) \
     func("BRAILLE", "Braille", REPAN_UN_BRAILLE, REPAN_U_DEFINE(BRAILLE)) \
     func("CONTROL", "Control", REPAN_UN_CONTROL, REPAN_U_DEFINE(CC)) \
     func("CYPRIOT", "Cypriot", REPAN_UN_CYPRIOT, REPAN_U_DEFINE(CYPRIOT)) \
     func("DESERET", "Deseret", REPAN_UN_DESERET, REPAN_U_DEFINE(DESERET)) \
     func("ELBASAN", "Elbasan", REPAN_UN_ELBASAN, REPAN_U_DEFINE(ELBASAN)) \
     func("ELYMAIC", "Elymaic", REPAN_UN_ELYMAIC, REPAN_U_DEFINE(ELYMAIC)) \
     func("GRANTHA", "Grantha", REPAN_UN_GRANTHA, REPAN_U_DEFINE(GRANTHA)) \
     func("HANUNOO", "Hanunoo", REPAN_UN_HANUNOO, REPAN_U_DEFINE(HANUNOO)) \
     func("KANNADA", "Kannada", REPAN_UN_KANNADA, REPAN_U_DEFINE(KANNADA)) \
     func("KAYAHLI", "Kayah_Li", REPAN_UN_KAYAH_LI, REPAN_U_DEFINE(KAYAH_LI)) \
     func("LINEARA", "Linear_A", REPAN_UN_LINEAR_A, REPAN_U_DEFINE(LINEAR_A)) \
     func("LINEARB", "Linear_B", REPAN_UN_LINEAR_B, REPAN_U_DEFINE(LINEAR_B)) \
     func("MAKASAR", "Makasar", REPAN_UN_MAKASAR, REPAN_U_DEFINE(MAKASAR)) \
     func("MANDAIC", "Mandaic", REPAN_UN_MANDAIC, REPAN_U_DEFINE(MANDAIC)) \
     func("MARCHEN", "Marchen", REPAN_UN_MARCHEN, REPAN_U_DEFINE(MARCHEN)) \
     func("MULTANI", "Multani", REPAN_UN_MULTANI, REPAN_U_DEFINE(MULTANI)) \
     func("MYANMAR", "Myanmar", REPAN_UN_MYANMAR, REPAN_U_DEFINE(MYANMAR)) \
     func("OLCHIKI", "Ol_Chiki", REPAN_UN_OL_CHIKI, REPAN_U_DEFINE(OL_CHIKI)) \
     func("OSMANYA", "Osmanya", REPAN_UN_OSMANYA, REPAN_U_DEFINE(OSMANYA)) \
     func("PHAGSPA", "Phags_Pa", REPAN_UN_PHAGS_PA, REPAN_U_DEFINE(PHAGS_PA)) \
     func("SHARADA", "Sharada", REPAN_UN_SHARADA, REPAN_U_DEFINE(SHARADA)) \
     func("SHAVIAN", "Shavian", REPAN_UN_SHAVIAN, REPAN_U_DEFINE(SHAVIAN)) \
     func("SIDDHAM", "Siddham", REPAN_UN_SIDDHAM, REPAN_U_DEFINE(SIDDHAM)) \
     func("SINHALA", "Sinhala", REPAN_UN_SINHALA, REPAN_U_DEFINE(SINHALA)) \
     func("SOGDIAN", "Sogdian", REPAN_UN_SOGDIAN, REPAN_U_DEFINE(SOGDIAN)) \
     func("SOYOMBO", "Soyombo", REPAN_UN_SOYOMBO, REPAN_U_DEFINE(SOYOMBO)) \
     func("TAGALOG", "Tagalog", REPAN_UN_TAGALOG, REPAN_U_DEFINE(TAGALOG)) \
     func("TAITHAM", "Tai_Tham", REPAN_UN_TAI_THAM, REPAN_U_DEFINE(TAI_THAM)) \
     func("TAIVIET", "Tai_Viet", REPAN_UN_TAI_VIET, REPAN_U_DEFINE(TAI_VIET)) \
     func("TIBETAN", "Tibetan", REPAN_UN_TIBETAN, REPAN_U_DEFINE(TIBETAN)) \
     func("TIRHUTA", "Tirhuta", REPAN_UN_TIRHUTA, REPAN_U_DEFINE(TIRHUTA)) \
     func("ARMENIAN", "Armenian", REPAN_UN_ARMENIAN, REPAN_U_DEFINE(ARMENIAN)) \
     func("ASSIGNED", "Assigned", REPAN_UN_ASSIGNED, REPAN_U_DEFINE(ASSIGNED)) \
     func("BALINESE", "Balinese", REPAN_UN_BALINESE, REPAN_U_DEFINE(BALINESE)) \
     func("BASSAVAH", "Bassa_Vah", REPAN_UN_BASSA_VAH, REPAN_U_DEFINE(BASSA_VAH)) \
     func("BOPOMOFO", "Bopomofo", REPAN_UN_BOPOMOFO, REPAN_U_DEFINE(BOPOMOFO)) \
     func("BUGINESE", "Buginese", REPAN_UN_BUGINESE, REPAN_U_DEFINE(BUGINESE)) \
     func("CHEROKEE", "Cherokee", REPAN_UN_CHEROKEE, REPAN_U_DEFINE(CHEROKEE)) \
     func("CYRILLIC", "Cyrillic", REPAN_UN_CYRILLIC, REPAN_U_DEFINE(CYRILLIC)) \
     func("DUPLOYAN", "Duployan", REPAN_UN_DUPLOYAN, REPAN_U_DEFINE(DUPLOYAN)) \
     func("ETHIOPIC", "Ethiopic", REPAN_UN_ETHIOPIC, REPAN_U_DEFINE(ETHIOPIC)) \
     func("GEORGIAN", "Georgian", REPAN_UN_GEORGIAN, REPAN_U_DEFINE(GEORGIAN)) \
     func("GUJARATI", "Gujarati", REPAN_UN_GUJARATI, REPAN_U_DEFINE(GUJARATI)) \
     func("GURMUKHI", "Gurmukhi", REPAN_UN_GURMUKHI, REPAN_U_DEFINE(GURMUKHI)) \
     func("HIRAGANA", "Hiragana", REPAN_UN_HIRAGANA, REPAN_U_DEFINE(HIRAGANA)) \
     func("JAVANESE", "Javanese", REPAN_UN_JAVANESE, REPAN_U_DEFINE(JAVANESE)) \
     func("KATAKANA", "Katakana", REPAN_UN_KATAKANA, REPAN_U_DEFINE(KATAKANA)) \
     func("MAHAJANI", "Mahajani", REPAN_UN_MAHAJANI, REPAN_U_DEFINE(MAHAJANI)) \
     func("TAGBANWA", "Tagbanwa", REPAN_UN_TAGBANWA, REPAN_U_DEFINE(TAGBANWA)) \
     func("TIFINAGH", "Tifinagh", REPAN_UN_TIFINAGH, REPAN_U_DEFINE(TIFINAGH)) \
     func("UGARITIC", "Ugaritic", REPAN_UN_UGARITIC, REPAN_U_DEFINE(UGARITIC)) \
     func("BHAIKSUKI", "Bhaiksuki", REPAN_UN_BHAIKSUKI, REPAN_U_DEFINE(BHAIKSUKI)) \
     func("CUNEIFORM", "Cuneiform", REPAN_UN_CUNEIFORM, REPAN_U_DEFINE(CUNEIFORM)) \
     func("INHERITED", "Inherited", REPAN_UN_INHERITED, REPAN_U_DEFINE(INHERITED)) \
     func("KHUDAWADI", "Khudawadi", REPAN_UN_KHUDAWADI, REPAN_U_DEFINE(KHUDAWADI)) \
     func("MALAYALAM", "Malayalam", REPAN_UN_MALAYALAM, REPAN_U_DEFINE(MALAYALAM)) \
     func("MONGOLIAN", "Mongolian", REPAN_UN_MONGOLIAN, REPAN_U_DEFINE(MONGOLIAN)) \
     func("NABATAEAN", "Nabataean", REPAN_UN_NABATAEAN, REPAN_U_DEFINE(NABATAEAN)) \
     func("NEWTAILUE", "New_Tai_Lue", REPAN_UN_NEW_TAI_LUE, REPAN_U_DEFINE(NEW_TAI_LUE)) \
     func("OLDITALIC", "Old_Italic", REPAN_UN_OLD_ITALIC, REPAN_U_DEFINE(OLD_ITALIC)) \
     func("OLDPERMIC", "Old_Permic", REPAN_UN_OLD_PERMIC, REPAN_U_DEFINE(OLD_PERMIC)) \
     func("OLDTURKIC", "Old_Turkic", REPAN_UN_OLD_TURKIC, REPAN_U_DEFINE(OLD_TURKIC)) \
     func("PALMYRENE", "Palmyrene", REPAN_UN_PALMYRENE, REPAN_U_DEFINE(PALMYRENE)) \
     func("PAUCINHAU", "Pau_Cin_Hau", REPAN_UN_PAU_CIN_HAU, REPAN_U_DEFINE(PAU_CIN_HAU)) \
     func("SAMARITAN", "Samaritan", REPAN_UN_SAMARITAN, REPAN_U_DEFINE(SAMARITAN)) \
     func("SEPARATOR", "Separator", REPAN_UN_SEPARATOR, REPAN_U_DEFINE(Z)) \
     func("SUNDANESE", "Sundanese", REPAN_UN_SUNDANESE, REPAN_U_DEFINE(SUNDANESE)) \
     func("SURROGATE", "Surrogate", REPAN_UN_SURROGATE, REPAN_U_DEFINE(CS)) \
     func("DEVANAGARI", "Devanagari", REPAN_UN_DEVANAGARI, REPAN_U_DEFINE(DEVANAGARI)) \
     func("GLAGOLITIC", "Glagolitic", REPAN_UN_GLAGOLITIC, REPAN_U_DEFINE(GLAGOLITIC)) \
     func("KHAROSHTHI", "Kharoshthi", REPAN_UN_KHAROSHTHI, REPAN_U_DEFINE(KHAROSHTHI)) \
     func("MANICHAEAN", "Manichaean", REPAN_UN_MANICHAEAN, REPAN_U_DEFINE(MANICHAEAN)) \
     func("MATHSYMBOL", "Math_Symbol", REPAN_UN_MATH_SYMBOL, REPAN_U_DEFINE(SM)) \
     func("OLDPERSIAN", "Old_Persian", REPAN_UN_OLD_PERSIAN, REPAN_U_DEFINE(OLD_PERSIAN)) \
     func("OLDSOGDIAN", "Old_Sogdian", REPAN_UN_OLD_SOGDIAN, REPAN_U_DEFINE(OLD_SOGDIAN)) \
     func("PHOENICIAN", "Phoenician", REPAN_UN_PHOENICIAN, REPAN_U_DEFINE(PHOENICIAN)) \
     func("PRIVATEUSE", "Private_Use", REPAN_UN_PRIVATE_USE, REPAN_U_DEFINE(CO)) \
     func("SAURASHTRA", "Saurashtra", REPAN_UN_SAURASHTRA, REPAN_U_DEFINE(SAURASHTRA)) \
     func("UNASSIGNED", "Unassigned", REPAN_UN_UNASSIGNED, REPAN_U_DEFINE(CN)) \
     func("WARANGCITI", "Warang_Citi", REPAN_UN_WARANG_CITI, REPAN_U_DEFINE(WARANG_CITI)) \
     func("CASEDLETTER", "Cased_Letter", REPAN_UN_CASED_LETTER, REPAN_U_DEFINE(L_AMPERSAND)) \
     func("MEDEFAIDRIN", "Medefaidrin", REPAN_UN_MEDEFAIDRIN, REPAN_U_DEFINE(MEDEFAIDRIN)) \
     func("MEETEIMAYEK", "Meetei_Mayek", REPAN_UN_MEETEI_MAYEK, REPAN_U_DEFINE(MEETEI_MAYEK)) \
     func("NANDINAGARI", "Nandinagari", REPAN_UN_NANDINAGARI, REPAN_U_DEFINE(NANDINAGARI)) \
     func("OTHERLETTER", "Other_Letter", REPAN_UN_OTHER_LETTER, REPAN_U_DEFINE(LO)) \
     func("OTHERNUMBER", "Other_Number", REPAN_UN_OTHER_NUMBER, REPAN_U_DEFINE(NO)) \
     func("OTHERSYMBOL", "Other_Symbol", REPAN_UN_OTHER_SYMBOL, REPAN_U_DEFINE(SO)) \
     func("PAHAWHHMONG", "Pahawh_Hmong", REPAN_UN_PAHAWH_HMONG, REPAN_U_DEFINE(PAHAWH_HMONG)) \
     func("PUNCTUATION", "Punctuation", REPAN_UN_PUNCTUATION, REPAN_U_DEFINE(P)) \
     func("SIGNWRITING", "SignWriting", REPAN_UN_SIGNWRITING, REPAN_U_DEFINE(SIGNWRITING)) \
     func("SORASOMPENG", "Sora_Sompeng", REPAN_UN_SORA_SOMPENG, REPAN_U_DEFINE(SORA_SOMPENG)) \
     func("SPACINGMARK", "Spacing_Mark", REPAN_UN_SPACING_MARK, REPAN_U_DEFINE(MC)) \
     func("SYLOTINAGRI", "Syloti_Nagri", REPAN_UN_SYLOTI_NAGRI, REPAN_U_DEFINE(SYLOTI_NAGRI)) \
     func("GUNJALAGONDI", "Gunjala_Gondi", REPAN_UN_GUNJALA_GONDI, REPAN_U_DEFINE(GUNJALA_GONDI)) \
     func("LETTERNUMBER", "Letter_Number", REPAN_UN_LETTER_NUMBER, REPAN_U_DEFINE(NL)) \
     func("MASARAMGONDI", "Masaram_Gondi", REPAN_UN_MASARAM_GONDI, REPAN_U_DEFINE(MASARAM_GONDI)) \
     func("MENDEKIKAKUI", "Mende_Kikakui", REPAN_UN_MENDE_KIKAKUI, REPAN_U_DEFINE(MENDE_KIKAKUI)) \
     func("OLDHUNGARIAN", "Old_Hungarian", REPAN_UN_OLD_HUNGARIAN, REPAN_U_DEFINE(OLD_HUNGARIAN)) \
     func("COMBININGMARK", "Combining_Mark", REPAN_UN_COMBINING_MARK, REPAN_U_DEFINE(M)) \
     func("DECIMALNUMBER", "Decimal_Number", REPAN_UN_DECIMAL_NUMBER, REPAN_U_DEFINE(ND)) \
     func("ENCLOSINGMARK", "Enclosing_Mark", REPAN_UN_ENCLOSING_MARK, REPAN_U_DEFINE(ME)) \
     func("LINESEPARATOR", "Line_Separator", REPAN_UN_LINE_SEPARATOR, REPAN_U_DEFINE(ZL)) \
     func("CURRENCYSYMBOL", "Currency_Symbol", REPAN_UN_CURRENCY_SYMBOL, REPAN_U_DEFINE(SC)) \
     func("HANIFIROHINGYA", "Hanifi_Rohingya", REPAN_UN_HANIFI_ROHINGYA, REPAN_U_DEFINE(HANIFI_ROHINGYA)) \
     func("MODIFIERLETTER", "Modifier_Letter", REPAN_UN_MODIFIER_LETTER, REPAN_U_DEFINE(LM)) \
     func("MODIFIERSYMBOL", "Modifier_Symbol", REPAN_UN_MODIFIER_SYMBOL, REPAN_U_DEFINE(SK)) \
     func("NONSPACINGMARK", "Nonspacing_Mark", REPAN_UN_NONSPACING_MARK, REPAN_U_DEFINE(MN)) \
     func("PSALTERPAHLAVI", "Psalter_Pahlavi", REPAN_UN_PSALTER_PAHLAVI, REPAN_U_DEFINE(PSALTER_PAHLAVI)) \
     func("SPACESEPARATOR", "Space_Separator", REPAN_UN_SPACE_SEPARATOR, REPAN_U_DEFINE(ZS)) \
     func("DASHPUNCTUATION", "Dash_Punctuation", REPAN_UN_DASH_PUNCTUATION, REPAN_U_DEFINE(PD)) \
     func("IMPERIALARAMAIC", "Imperial_Aramaic", REPAN_UN_IMPERIAL_ARAMAIC, REPAN_U_DEFINE(IMPERIAL_ARAMAIC)) \
     func("LOWERCASELETTER", "Lowercase_Letter", REPAN_UN_LOWERCASE_LETTER, REPAN_U_DEFINE(LL)) \
     func("MEROITICCURSIVE", "Meroitic_Cursive", REPAN_UN_MEROITIC_CURSIVE, REPAN_U_DEFINE(MEROITIC_CURSIVE)) \
     func("OLDNORTHARABIAN", "Old_North_Arabian", REPAN_UN_OLD_NORTH_ARABIAN, REPAN_U_DEFINE(OLD_NORTH_ARABIAN)) \
     func("OLDSOUTHARABIAN", "Old_South_Arabian", REPAN_UN_OLD_SOUTH_ARABIAN, REPAN_U_DEFINE(OLD_SOUTH_ARABIAN)) \
     func("OPENPUNCTUATION", "Open_Punctuation", REPAN_UN_OPEN_PUNCTUATION, REPAN_U_DEFINE(PS)) \
     func("TITLECASELETTER", "Titlecase_Letter", REPAN_UN_TITLECASE_LETTER, REPAN_U_DEFINE(LT)) \
     func("UPPERCASELETTER", "Uppercase_Letter", REPAN_UN_UPPERCASE_LETTER, REPAN_U_DEFINE(LU)) \
     func("ZANABAZARSQUARE", "Zanabazar_Square", REPAN_UN_ZANABAZAR_SQUARE, REPAN_U_DEFINE(ZANABAZAR_SQUARE)) \
     func("CLOSEPUNCTUATION", "Close_Punctuation", REPAN_UN_CLOSE_PUNCTUATION, REPAN_U_DEFINE(PE)) \
     func("FINALPUNCTUATION", "Final_Punctuation", REPAN_UN_FINAL_PUNCTUATION, REPAN_U_DEFINE(PF)) \
     func("OTHERPUNCTUATION", "Other_Punctuation", REPAN_UN_OTHER_PUNCTUATION, REPAN_U_DEFINE(PO)) \
     func("CAUCASIANALBANIAN", "Caucasian_Albanian", REPAN_UN_CAUCASIAN_ALBANIAN, REPAN_U_DEFINE(CAUCASIAN_ALBANIAN)) \
     func("CANADIANABORIGINAL", "Canadian_Aboriginal", REPAN_UN_CANADIAN_ABORIGINAL, REPAN_U_DEFINE(CANADIAN_ABORIGINAL)) \
     func("INITIALPUNCTUATION", "Initial_Punctuation", REPAN_UN_INITIAL_PUNCTUATION, REPAN_U_DEFINE(PI)) \
     func("PARAGRAPHSEPARATOR", "Paragraph_Separator", REPAN_UN_PARAGRAPH_SEPARATOR, REPAN_U_DEFINE(ZP)) \
     func("EGYPTIANHIEROGLYPHS", "Egyptian_Hieroglyphs", REPAN_UN_EGYPTIAN_HIEROGLYPHS, REPAN_U_DEFINE(EGYPTIAN_HIEROGLYPHS)) \
     func("MEROITICHIEROGLYPHS", "Meroitic_Hieroglyphs", REPAN_UN_MEROITIC_HIEROGLYPHS, REPAN_U_DEFINE(MEROITIC_HIEROGLYPHS)) \
     func("ANATOLIANHIEROGLYPHS", "Anatolian_Hieroglyphs", REPAN_UN_ANATOLIAN_HIEROGLYPHS, REPAN_U_DEFINE(ANATOLIAN_HIEROGLYPHS)) \
     func("CONNECTORPUNCTUATION", "Connector_Punctuation", REPAN_UN_CONNECTOR_PUNCTUATION, REPAN_U_DEFINE(PC)) \
     func("INSCRIPTIONALPAHLAVI", "Inscriptional_Pahlavi", REPAN_UN_INSCRIPTIONAL_PAHLAVI, REPAN_U_DEFINE(INSCRIPTIONAL_PAHLAVI)) \
     func("NYIAKENGPUACHUEHMONG", "Nyiakeng_Puachue_Hmong", REPAN_UN_NYIAKENG_PUACHUE_HMONG, REPAN_U_DEFINE(NYIAKENG_PUACHUE_HMONG)) \
     func("INSCRIPTIONALPARTHIAN", "Inscriptional_Parthian", REPAN_UN_INSCRIPTIONAL_PARTHIAN, REPAN_U_DEFINE(INSCRIPTIONAL_PARTHIAN))
