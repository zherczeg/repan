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
    REPAN_UC_Cn,
};

enum {
    REPAN_UP_ANY = 0,
    REPAN_UP_L_AMPERSAND = 3,
    REPAN_UP_L = 6,
    REPAN_UP_M = 9,
    REPAN_UP_N = 12,
    REPAN_UP_P = 15,
    REPAN_UP_S = 18,
    REPAN_UP_Z = 21,
    REPAN_UP_C = 24,
    REPAN_UP_ID_START = 27,
    REPAN_UP_ID_CONTINUE = 42,
};

#define REPAN_U_PROPERTIES(func) \
     func("C", REPAN_UC_NAME_C, REPAN_U_DEFINE_PROPERTY(C)) \
     func("L", REPAN_UC_NAME_L, REPAN_U_DEFINE_PROPERTY(L)) \
     func("M", REPAN_UC_NAME_M, REPAN_U_DEFINE_PROPERTY(M)) \
     func("N", REPAN_UC_NAME_N, REPAN_U_DEFINE_PROPERTY(N)) \
     func("P", REPAN_UC_NAME_P, REPAN_U_DEFINE_PROPERTY(P)) \
     func("S", REPAN_UC_NAME_S, REPAN_U_DEFINE_PROPERTY(S)) \
     func("Z", REPAN_UC_NAME_Z, REPAN_U_DEFINE_PROPERTY(Z)) \
     func("Cc", REPAN_UC_NAME_Cc, REPAN_U_DEFINE_CATHEGORY(Cc)) \
     func("Cf", REPAN_UC_NAME_Cf, REPAN_U_DEFINE_CATHEGORY(Cf)) \
     func("Cn", REPAN_UC_NAME_Cn, REPAN_U_DEFINE_CATHEGORY(Cn)) \
     func("Co", REPAN_UC_NAME_Co, REPAN_U_DEFINE_CATHEGORY(Co)) \
     func("Cs", REPAN_UC_NAME_Cs, REPAN_U_DEFINE_CATHEGORY(Cs)) \
     func("L&", REPAN_UC_NAME_L_AMPERSAND, REPAN_U_DEFINE_PROPERTY(L_AMPERSAND)) \
     func("Ll", REPAN_UC_NAME_Ll, REPAN_U_DEFINE_CATHEGORY(Ll)) \
     func("Lm", REPAN_UC_NAME_Lm, REPAN_U_DEFINE_CATHEGORY(Lm)) \
     func("Lo", REPAN_UC_NAME_Lo, REPAN_U_DEFINE_CATHEGORY(Lo)) \
     func("Lt", REPAN_UC_NAME_Lt, REPAN_U_DEFINE_CATHEGORY(Lt)) \
     func("Lu", REPAN_UC_NAME_Lu, REPAN_U_DEFINE_CATHEGORY(Lu)) \
     func("Mc", REPAN_UC_NAME_Mc, REPAN_U_DEFINE_CATHEGORY(Mc)) \
     func("Me", REPAN_UC_NAME_Me, REPAN_U_DEFINE_CATHEGORY(Me)) \
     func("Mn", REPAN_UC_NAME_Mn, REPAN_U_DEFINE_CATHEGORY(Mn)) \
     func("Nd", REPAN_UC_NAME_Nd, REPAN_U_DEFINE_CATHEGORY(Nd)) \
     func("Nl", REPAN_UC_NAME_Nl, REPAN_U_DEFINE_CATHEGORY(Nl)) \
     func("No", REPAN_UC_NAME_No, REPAN_U_DEFINE_CATHEGORY(No)) \
     func("Pc", REPAN_UC_NAME_Pc, REPAN_U_DEFINE_CATHEGORY(Pc)) \
     func("Pd", REPAN_UC_NAME_Pd, REPAN_U_DEFINE_CATHEGORY(Pd)) \
     func("Pe", REPAN_UC_NAME_Pe, REPAN_U_DEFINE_CATHEGORY(Pe)) \
     func("Pf", REPAN_UC_NAME_Pf, REPAN_U_DEFINE_CATHEGORY(Pf)) \
     func("Pi", REPAN_UC_NAME_Pi, REPAN_U_DEFINE_CATHEGORY(Pi)) \
     func("Po", REPAN_UC_NAME_Po, REPAN_U_DEFINE_CATHEGORY(Po)) \
     func("Ps", REPAN_UC_NAME_Ps, REPAN_U_DEFINE_CATHEGORY(Ps)) \
     func("Sc", REPAN_UC_NAME_Sc, REPAN_U_DEFINE_CATHEGORY(Sc)) \
     func("Sk", REPAN_UC_NAME_Sk, REPAN_U_DEFINE_CATHEGORY(Sk)) \
     func("Sm", REPAN_UC_NAME_Sm, REPAN_U_DEFINE_CATHEGORY(Sm)) \
     func("So", REPAN_UC_NAME_So, REPAN_U_DEFINE_CATHEGORY(So)) \
     func("Yi", REPAN_US_YI, REPAN_U_DEFINE_SCRIPT(YI)) \
     func("Zl", REPAN_UC_NAME_Zl, REPAN_U_DEFINE_CATHEGORY(Zl)) \
     func("Zp", REPAN_UC_NAME_Zp, REPAN_U_DEFINE_CATHEGORY(Zp)) \
     func("Zs", REPAN_UC_NAME_Zs, REPAN_U_DEFINE_CATHEGORY(Zs)) \
     func("Any", REPAN_UC_NAME_ANY, REPAN_U_DEFINE_PROPERTY(ANY)) \
     func("Han", REPAN_US_HAN, REPAN_U_DEFINE_SCRIPT(HAN)) \
     func("Lao", REPAN_US_LAO, REPAN_U_DEFINE_SCRIPT(LAO)) \
     func("Mro", REPAN_US_MRO, REPAN_U_DEFINE_SCRIPT(MRO)) \
     func("Nko", REPAN_US_NKO, REPAN_U_DEFINE_SCRIPT(NKO)) \
     func("Vai", REPAN_US_VAI, REPAN_U_DEFINE_SCRIPT(VAI)) \
     func("Ahom", REPAN_US_AHOM, REPAN_U_DEFINE_SCRIPT(AHOM)) \
     func("Cham", REPAN_US_CHAM, REPAN_U_DEFINE_SCRIPT(CHAM)) \
     func("Lisu", REPAN_US_LISU, REPAN_U_DEFINE_SCRIPT(LISU)) \
     func("Miao", REPAN_US_MIAO, REPAN_U_DEFINE_SCRIPT(MIAO)) \
     func("Modi", REPAN_US_MODI, REPAN_U_DEFINE_SCRIPT(MODI)) \
     func("Newa", REPAN_US_NEWA, REPAN_U_DEFINE_SCRIPT(NEWA)) \
     func("Thai", REPAN_US_THAI, REPAN_U_DEFINE_SCRIPT(THAI)) \
     func("Adlam", REPAN_US_ADLAM, REPAN_U_DEFINE_SCRIPT(ADLAM)) \
     func("Bamum", REPAN_US_BAMUM, REPAN_U_DEFINE_SCRIPT(BAMUM)) \
     func("Batak", REPAN_US_BATAK, REPAN_U_DEFINE_SCRIPT(BATAK)) \
     func("Buhid", REPAN_US_BUHID, REPAN_U_DEFINE_SCRIPT(BUHID)) \
     func("Dogra", REPAN_US_DOGRA, REPAN_U_DEFINE_SCRIPT(DOGRA)) \
     func("Greek", REPAN_US_GREEK, REPAN_U_DEFINE_SCRIPT(GREEK)) \
     func("Khmer", REPAN_US_KHMER, REPAN_U_DEFINE_SCRIPT(KHMER)) \
     func("Latin", REPAN_US_LATIN, REPAN_U_DEFINE_SCRIPT(LATIN)) \
     func("Limbu", REPAN_US_LIMBU, REPAN_U_DEFINE_SCRIPT(LIMBU)) \
     func("Nushu", REPAN_US_NUSHU, REPAN_U_DEFINE_SCRIPT(NUSHU)) \
     func("Ogham", REPAN_US_OGHAM, REPAN_U_DEFINE_SCRIPT(OGHAM)) \
     func("Oriya", REPAN_US_ORIYA, REPAN_U_DEFINE_SCRIPT(ORIYA)) \
     func("Osage", REPAN_US_OSAGE, REPAN_U_DEFINE_SCRIPT(OSAGE)) \
     func("Runic", REPAN_US_RUNIC, REPAN_U_DEFINE_SCRIPT(RUNIC)) \
     func("Takri", REPAN_US_TAKRI, REPAN_U_DEFINE_SCRIPT(TAKRI)) \
     func("Tamil", REPAN_US_TAMIL, REPAN_U_DEFINE_SCRIPT(TAMIL)) \
     func("Arabic", REPAN_US_ARABIC, REPAN_U_DEFINE_SCRIPT(ARABIC)) \
     func("Brahmi", REPAN_US_BRAHMI, REPAN_U_DEFINE_SCRIPT(BRAHMI)) \
     func("Carian", REPAN_US_CARIAN, REPAN_U_DEFINE_SCRIPT(CARIAN)) \
     func("Chakma", REPAN_US_CHAKMA, REPAN_U_DEFINE_SCRIPT(CHAKMA)) \
     func("Common", REPAN_US_COMMON, REPAN_U_DEFINE_SCRIPT(COMMON)) \
     func("Coptic", REPAN_US_COPTIC, REPAN_U_DEFINE_SCRIPT(COPTIC)) \
     func("Gothic", REPAN_US_GOTHIC, REPAN_U_DEFINE_SCRIPT(GOTHIC)) \
     func("Hangul", REPAN_US_HANGUL, REPAN_U_DEFINE_SCRIPT(HANGUL)) \
     func("Hatran", REPAN_US_HATRAN, REPAN_U_DEFINE_SCRIPT(HATRAN)) \
     func("Hebrew", REPAN_US_HEBREW, REPAN_U_DEFINE_SCRIPT(HEBREW)) \
     func("Kaithi", REPAN_US_KAITHI, REPAN_U_DEFINE_SCRIPT(KAITHI)) \
     func("Khojki", REPAN_US_KHOJKI, REPAN_U_DEFINE_SCRIPT(KHOJKI)) \
     func("Lepcha", REPAN_US_LEPCHA, REPAN_U_DEFINE_SCRIPT(LEPCHA)) \
     func("Lycian", REPAN_US_LYCIAN, REPAN_U_DEFINE_SCRIPT(LYCIAN)) \
     func("Lydian", REPAN_US_LYDIAN, REPAN_U_DEFINE_SCRIPT(LYDIAN)) \
     func("Rejang", REPAN_US_REJANG, REPAN_U_DEFINE_SCRIPT(REJANG)) \
     func("Syriac", REPAN_US_SYRIAC, REPAN_U_DEFINE_SCRIPT(SYRIAC)) \
     func("Tai_Le", REPAN_US_TAI_LE, REPAN_U_DEFINE_SCRIPT(TAI_LE)) \
     func("Tangut", REPAN_US_TANGUT, REPAN_U_DEFINE_SCRIPT(TANGUT)) \
     func("Telugu", REPAN_US_TELUGU, REPAN_U_DEFINE_SCRIPT(TELUGU)) \
     func("Thaana", REPAN_US_THAANA, REPAN_U_DEFINE_SCRIPT(THAANA)) \
     func("Wancho", REPAN_US_WANCHO, REPAN_U_DEFINE_SCRIPT(WANCHO)) \
     func("Avestan", REPAN_US_AVESTAN, REPAN_U_DEFINE_SCRIPT(AVESTAN)) \
     func("Bengali", REPAN_US_BENGALI, REPAN_U_DEFINE_SCRIPT(BENGALI)) \
     func("Braille", REPAN_US_BRAILLE, REPAN_U_DEFINE_SCRIPT(BRAILLE)) \
     func("Cypriot", REPAN_US_CYPRIOT, REPAN_U_DEFINE_SCRIPT(CYPRIOT)) \
     func("Deseret", REPAN_US_DESERET, REPAN_U_DEFINE_SCRIPT(DESERET)) \
     func("Elbasan", REPAN_US_ELBASAN, REPAN_U_DEFINE_SCRIPT(ELBASAN)) \
     func("Elymaic", REPAN_US_ELYMAIC, REPAN_U_DEFINE_SCRIPT(ELYMAIC)) \
     func("Grantha", REPAN_US_GRANTHA, REPAN_U_DEFINE_SCRIPT(GRANTHA)) \
     func("Hanunoo", REPAN_US_HANUNOO, REPAN_U_DEFINE_SCRIPT(HANUNOO)) \
     func("Kannada", REPAN_US_KANNADA, REPAN_U_DEFINE_SCRIPT(KANNADA)) \
     func("Makasar", REPAN_US_MAKASAR, REPAN_U_DEFINE_SCRIPT(MAKASAR)) \
     func("Mandaic", REPAN_US_MANDAIC, REPAN_U_DEFINE_SCRIPT(MANDAIC)) \
     func("Marchen", REPAN_US_MARCHEN, REPAN_U_DEFINE_SCRIPT(MARCHEN)) \
     func("Multani", REPAN_US_MULTANI, REPAN_U_DEFINE_SCRIPT(MULTANI)) \
     func("Myanmar", REPAN_US_MYANMAR, REPAN_U_DEFINE_SCRIPT(MYANMAR)) \
     func("Osmanya", REPAN_US_OSMANYA, REPAN_U_DEFINE_SCRIPT(OSMANYA)) \
     func("Sharada", REPAN_US_SHARADA, REPAN_U_DEFINE_SCRIPT(SHARADA)) \
     func("Shavian", REPAN_US_SHAVIAN, REPAN_U_DEFINE_SCRIPT(SHAVIAN)) \
     func("Siddham", REPAN_US_SIDDHAM, REPAN_U_DEFINE_SCRIPT(SIDDHAM)) \
     func("Sinhala", REPAN_US_SINHALA, REPAN_U_DEFINE_SCRIPT(SINHALA)) \
     func("Sogdian", REPAN_US_SOGDIAN, REPAN_U_DEFINE_SCRIPT(SOGDIAN)) \
     func("Soyombo", REPAN_US_SOYOMBO, REPAN_U_DEFINE_SCRIPT(SOYOMBO)) \
     func("Tagalog", REPAN_US_TAGALOG, REPAN_U_DEFINE_SCRIPT(TAGALOG)) \
     func("Tibetan", REPAN_US_TIBETAN, REPAN_U_DEFINE_SCRIPT(TIBETAN)) \
     func("Tirhuta", REPAN_US_TIRHUTA, REPAN_U_DEFINE_SCRIPT(TIRHUTA)) \
     func("Armenian", REPAN_US_ARMENIAN, REPAN_U_DEFINE_SCRIPT(ARMENIAN)) \
     func("Balinese", REPAN_US_BALINESE, REPAN_U_DEFINE_SCRIPT(BALINESE)) \
     func("Bopomofo", REPAN_US_BOPOMOFO, REPAN_U_DEFINE_SCRIPT(BOPOMOFO)) \
     func("Buginese", REPAN_US_BUGINESE, REPAN_U_DEFINE_SCRIPT(BUGINESE)) \
     func("Cherokee", REPAN_US_CHEROKEE, REPAN_U_DEFINE_SCRIPT(CHEROKEE)) \
     func("Cyrillic", REPAN_US_CYRILLIC, REPAN_U_DEFINE_SCRIPT(CYRILLIC)) \
     func("Duployan", REPAN_US_DUPLOYAN, REPAN_U_DEFINE_SCRIPT(DUPLOYAN)) \
     func("Ethiopic", REPAN_US_ETHIOPIC, REPAN_U_DEFINE_SCRIPT(ETHIOPIC)) \
     func("Georgian", REPAN_US_GEORGIAN, REPAN_U_DEFINE_SCRIPT(GEORGIAN)) \
     func("Gujarati", REPAN_US_GUJARATI, REPAN_U_DEFINE_SCRIPT(GUJARATI)) \
     func("Gurmukhi", REPAN_US_GURMUKHI, REPAN_U_DEFINE_SCRIPT(GURMUKHI)) \
     func("Hiragana", REPAN_US_HIRAGANA, REPAN_U_DEFINE_SCRIPT(HIRAGANA)) \
     func("Javanese", REPAN_US_JAVANESE, REPAN_U_DEFINE_SCRIPT(JAVANESE)) \
     func("Katakana", REPAN_US_KATAKANA, REPAN_U_DEFINE_SCRIPT(KATAKANA)) \
     func("Kayah_Li", REPAN_US_KAYAH_LI, REPAN_U_DEFINE_SCRIPT(KAYAH_LI)) \
     func("Linear_A", REPAN_US_LINEAR_A, REPAN_U_DEFINE_SCRIPT(LINEAR_A)) \
     func("Linear_B", REPAN_US_LINEAR_B, REPAN_U_DEFINE_SCRIPT(LINEAR_B)) \
     func("Mahajani", REPAN_US_MAHAJANI, REPAN_U_DEFINE_SCRIPT(MAHAJANI)) \
     func("Ol_Chiki", REPAN_US_OL_CHIKI, REPAN_U_DEFINE_SCRIPT(OL_CHIKI)) \
     func("Phags_Pa", REPAN_US_PHAGS_PA, REPAN_U_DEFINE_SCRIPT(PHAGS_PA)) \
     func("Tagbanwa", REPAN_US_TAGBANWA, REPAN_U_DEFINE_SCRIPT(TAGBANWA)) \
     func("Tai_Tham", REPAN_US_TAI_THAM, REPAN_U_DEFINE_SCRIPT(TAI_THAM)) \
     func("Tai_Viet", REPAN_US_TAI_VIET, REPAN_U_DEFINE_SCRIPT(TAI_VIET)) \
     func("Tifinagh", REPAN_US_TIFINAGH, REPAN_U_DEFINE_SCRIPT(TIFINAGH)) \
     func("Ugaritic", REPAN_US_UGARITIC, REPAN_U_DEFINE_SCRIPT(UGARITIC)) \
     func("Bassa_Vah", REPAN_US_BASSA_VAH, REPAN_U_DEFINE_SCRIPT(BASSA_VAH)) \
     func("Bhaiksuki", REPAN_US_BHAIKSUKI, REPAN_U_DEFINE_SCRIPT(BHAIKSUKI)) \
     func("Cuneiform", REPAN_US_CUNEIFORM, REPAN_U_DEFINE_SCRIPT(CUNEIFORM)) \
     func("Inherited", REPAN_US_INHERITED, REPAN_U_DEFINE_SCRIPT(INHERITED)) \
     func("Khudawadi", REPAN_US_KHUDAWADI, REPAN_U_DEFINE_SCRIPT(KHUDAWADI)) \
     func("Malayalam", REPAN_US_MALAYALAM, REPAN_U_DEFINE_SCRIPT(MALAYALAM)) \
     func("Mongolian", REPAN_US_MONGOLIAN, REPAN_U_DEFINE_SCRIPT(MONGOLIAN)) \
     func("Nabataean", REPAN_US_NABATAEAN, REPAN_U_DEFINE_SCRIPT(NABATAEAN)) \
     func("Palmyrene", REPAN_US_PALMYRENE, REPAN_U_DEFINE_SCRIPT(PALMYRENE)) \
     func("Samaritan", REPAN_US_SAMARITAN, REPAN_U_DEFINE_SCRIPT(SAMARITAN)) \
     func("Sundanese", REPAN_US_SUNDANESE, REPAN_U_DEFINE_SCRIPT(SUNDANESE)) \
     func("Devanagari", REPAN_US_DEVANAGARI, REPAN_U_DEFINE_SCRIPT(DEVANAGARI)) \
     func("Glagolitic", REPAN_US_GLAGOLITIC, REPAN_U_DEFINE_SCRIPT(GLAGOLITIC)) \
     func("Kharoshthi", REPAN_US_KHAROSHTHI, REPAN_U_DEFINE_SCRIPT(KHAROSHTHI)) \
     func("Manichaean", REPAN_US_MANICHAEAN, REPAN_U_DEFINE_SCRIPT(MANICHAEAN)) \
     func("Old_Italic", REPAN_US_OLD_ITALIC, REPAN_U_DEFINE_SCRIPT(OLD_ITALIC)) \
     func("Old_Permic", REPAN_US_OLD_PERMIC, REPAN_U_DEFINE_SCRIPT(OLD_PERMIC)) \
     func("Old_Turkic", REPAN_US_OLD_TURKIC, REPAN_U_DEFINE_SCRIPT(OLD_TURKIC)) \
     func("Phoenician", REPAN_US_PHOENICIAN, REPAN_U_DEFINE_SCRIPT(PHOENICIAN)) \
     func("Saurashtra", REPAN_US_SAURASHTRA, REPAN_U_DEFINE_SCRIPT(SAURASHTRA)) \
     func("Medefaidrin", REPAN_US_MEDEFAIDRIN, REPAN_U_DEFINE_SCRIPT(MEDEFAIDRIN)) \
     func("Nandinagari", REPAN_US_NANDINAGARI, REPAN_U_DEFINE_SCRIPT(NANDINAGARI)) \
     func("New_Tai_Lue", REPAN_US_NEW_TAI_LUE, REPAN_U_DEFINE_SCRIPT(NEW_TAI_LUE)) \
     func("Old_Persian", REPAN_US_OLD_PERSIAN, REPAN_U_DEFINE_SCRIPT(OLD_PERSIAN)) \
     func("Old_Sogdian", REPAN_US_OLD_SOGDIAN, REPAN_U_DEFINE_SCRIPT(OLD_SOGDIAN)) \
     func("Pau_Cin_Hau", REPAN_US_PAU_CIN_HAU, REPAN_U_DEFINE_SCRIPT(PAU_CIN_HAU)) \
     func("SignWriting", REPAN_US_SIGNWRITING, REPAN_U_DEFINE_SCRIPT(SIGNWRITING)) \
     func("Warang_Citi", REPAN_US_WARANG_CITI, REPAN_U_DEFINE_SCRIPT(WARANG_CITI)) \
     func("Meetei_Mayek", REPAN_US_MEETEI_MAYEK, REPAN_U_DEFINE_SCRIPT(MEETEI_MAYEK)) \
     func("Pahawh_Hmong", REPAN_US_PAHAWH_HMONG, REPAN_U_DEFINE_SCRIPT(PAHAWH_HMONG)) \
     func("Sora_Sompeng", REPAN_US_SORA_SOMPENG, REPAN_U_DEFINE_SCRIPT(SORA_SOMPENG)) \
     func("Syloti_Nagri", REPAN_US_SYLOTI_NAGRI, REPAN_U_DEFINE_SCRIPT(SYLOTI_NAGRI)) \
     func("Gunjala_Gondi", REPAN_US_GUNJALA_GONDI, REPAN_U_DEFINE_SCRIPT(GUNJALA_GONDI)) \
     func("Masaram_Gondi", REPAN_US_MASARAM_GONDI, REPAN_U_DEFINE_SCRIPT(MASARAM_GONDI)) \
     func("Mende_Kikakui", REPAN_US_MENDE_KIKAKUI, REPAN_U_DEFINE_SCRIPT(MENDE_KIKAKUI)) \
     func("Old_Hungarian", REPAN_US_OLD_HUNGARIAN, REPAN_U_DEFINE_SCRIPT(OLD_HUNGARIAN)) \
     func("Hanifi_Rohingya", REPAN_US_HANIFI_ROHINGYA, REPAN_U_DEFINE_SCRIPT(HANIFI_ROHINGYA)) \
     func("Psalter_Pahlavi", REPAN_US_PSALTER_PAHLAVI, REPAN_U_DEFINE_SCRIPT(PSALTER_PAHLAVI)) \
     func("Imperial_Aramaic", REPAN_US_IMPERIAL_ARAMAIC, REPAN_U_DEFINE_SCRIPT(IMPERIAL_ARAMAIC)) \
     func("Meroitic_Cursive", REPAN_US_MEROITIC_CURSIVE, REPAN_U_DEFINE_SCRIPT(MEROITIC_CURSIVE)) \
     func("Zanabazar_Square", REPAN_US_ZANABAZAR_SQUARE, REPAN_U_DEFINE_SCRIPT(ZANABAZAR_SQUARE)) \
     func("Old_North_Arabian", REPAN_US_OLD_NORTH_ARABIAN, REPAN_U_DEFINE_SCRIPT(OLD_NORTH_ARABIAN)) \
     func("Old_South_Arabian", REPAN_US_OLD_SOUTH_ARABIAN, REPAN_U_DEFINE_SCRIPT(OLD_SOUTH_ARABIAN)) \
     func("Caucasian_Albanian", REPAN_US_CAUCASIAN_ALBANIAN, REPAN_U_DEFINE_SCRIPT(CAUCASIAN_ALBANIAN)) \
     func("Canadian_Aboriginal", REPAN_US_CANADIAN_ABORIGINAL, REPAN_U_DEFINE_SCRIPT(CANADIAN_ABORIGINAL)) \
     func("Egyptian_Hieroglyphs", REPAN_US_EGYPTIAN_HIEROGLYPHS, REPAN_U_DEFINE_SCRIPT(EGYPTIAN_HIEROGLYPHS)) \
     func("Meroitic_Hieroglyphs", REPAN_US_MEROITIC_HIEROGLYPHS, REPAN_U_DEFINE_SCRIPT(MEROITIC_HIEROGLYPHS)) \
     func("Anatolian_Hieroglyphs", REPAN_US_ANATOLIAN_HIEROGLYPHS, REPAN_U_DEFINE_SCRIPT(ANATOLIAN_HIEROGLYPHS)) \
     func("Inscriptional_Pahlavi", REPAN_US_INSCRIPTIONAL_PAHLAVI, REPAN_U_DEFINE_SCRIPT(INSCRIPTIONAL_PAHLAVI)) \
     func("Inscriptional_Parthian", REPAN_US_INSCRIPTIONAL_PARTHIAN, REPAN_U_DEFINE_SCRIPT(INSCRIPTIONAL_PARTHIAN)) \
     func("Nyiakeng_Puachue_Hmong", REPAN_US_NYIAKENG_PUACHUE_HMONG, REPAN_U_DEFINE_SCRIPT(NYIAKENG_PUACHUE_HMONG))
