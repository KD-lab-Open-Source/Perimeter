#include <codecvt>
#include <locale>
#include <unordered_map>

#include "xutl.h"
#include "codepages.h"

bool isCodepageInit = false;
typedef std::unordered_map<char32_t, uint8_t> CodepageMap;
CodepageMap codepage_windows1250;
CodepageMap codepage_windows1251;
std::unordered_map<uint32_t, char32_t> utf32_codepages;

void initCodePages();

uint16_t localeToCodepage(const std::string& locale) {
    //Russian language uses 1251, the rest of languages which game was released uses 1250
    if (startsWith(locale, "russian")) {
        return 1251;
    } else {
        return 1250;
    }
}

std::string convertToCodepage(const char* utf8, uint16_t codepage) {
    if (!isCodepageInit) {
        initCodePages();
    }
    std::string result;
    if (!utf8 || codepage == 0) return result;

    CodepageMap* map;
    switch (codepage) {
        case 1251:
            map = &codepage_windows1251;
            break;
        case 1250:
        default:
            map = &codepage_windows1250;
            break;
    }

    //Convert UTF-8 into UTF-32
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8cvt;
    std::u32string conv = utf8cvt.from_bytes(utf8);

    //Map each UTF-32 into codepage character if any
    for (char32_t input : conv) {
        if (map->count(input)) {
            result += static_cast<char>(map->at(input));
        } else {
            result += '?';
        }
    }

    return result;
}

std::string convertToUnicode(const std::string& str, uint16_t codepage) {
    if (!isCodepageInit) {
        initCodePages();
    }
    if (str.empty() || codepage == 0) return str;
    
    //Convert each codepage char into UTF-32 
    std::u32string conv;
    uint32_t pageid = codepage << 16;
    for (uint8_t input : str) {
        uint32_t index = pageid | input;
        if (utf32_codepages.count(index)) {
            conv += utf32_codepages.at(index);
        } else {
            conv += '?';
        }
    }

    //Convert UTF-32 into UTF-8
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8cvt;
    std::string result = utf8cvt.to_bytes(conv);

    return result;
}

std::string convertToCodepage(const char* utf8, const std::string& locale) {
    return convertToCodepage(utf8, localeToCodepage(locale));
}

std::string convertToUnicode(const std::string& str, const std::string& locale) {
    return convertToUnicode(str, localeToCodepage(locale));
}

///Load UTF32 -> codepage as codepage + character -> UTF32
void loadUTFCodepage(const CodepageMap& codepageMap, uint16_t codepage) {
    uint32_t pageid = codepage << 16;
    for (auto& entry : codepageMap) {
        utf32_codepages[pageid | entry.second] = entry.first;
    }
}

/// Codepages - UTF32 tables setup
void initCodePages() {
    isCodepageInit = true;
    codepage_windows1250.clear();
    codepage_windows1251.clear();
    utf32_codepages.clear();

    /// Windows 1250
    //Source: https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WindowsBestFit/bestfit1250.txt
    codepage_windows1250[0x0000] = 0x00; //Null
    codepage_windows1250[0x0001] = 0x01; //Start Of Heading
    codepage_windows1250[0x0002] = 0x02; //Start Of Text
    codepage_windows1250[0x0003] = 0x03; //End Of Text
    codepage_windows1250[0x0004] = 0x04; //End Of Transmission
    codepage_windows1250[0x0005] = 0x05; //Enquiry
    codepage_windows1250[0x0006] = 0x06; //Acknowledge
    codepage_windows1250[0x0007] = 0x07; //Bell
    codepage_windows1250[0x0008] = 0x08; //Backspace
    codepage_windows1250[0x0009] = 0x09; //Horizontal Tabulation
    codepage_windows1250[0x000a] = 0x0a; //Line Feed
    codepage_windows1250[0x000b] = 0x0b; //Vertical Tabulation
    codepage_windows1250[0x000c] = 0x0c; //Form Feed
    codepage_windows1250[0x000d] = 0x0d; //Carriage Return
    codepage_windows1250[0x000e] = 0x0e; //Shift Out
    codepage_windows1250[0x000f] = 0x0f; //Shift In
    codepage_windows1250[0x0010] = 0x10; //Data Link Escape
    codepage_windows1250[0x0011] = 0x11; //Device Control One
    codepage_windows1250[0x0012] = 0x12; //Device Control Two
    codepage_windows1250[0x0013] = 0x13; //Device Control Three
    codepage_windows1250[0x0014] = 0x14; //Device Control Four
    codepage_windows1250[0x0015] = 0x15; //Negative Acknowledge
    codepage_windows1250[0x0016] = 0x16; //Synchronous Idle
    codepage_windows1250[0x0017] = 0x17; //End Of Transmission Block
    codepage_windows1250[0x0018] = 0x18; //Cancel
    codepage_windows1250[0x0019] = 0x19; //End Of Medium
    codepage_windows1250[0x001a] = 0x1a; //Substitute
    codepage_windows1250[0x001b] = 0x1b; //Escape
    codepage_windows1250[0x001c] = 0x1c; //File Separator
    codepage_windows1250[0x001d] = 0x1d; //Group Separator
    codepage_windows1250[0x001e] = 0x1e; //Record Separator
    codepage_windows1250[0x001f] = 0x1f; //Unit Separator
    codepage_windows1250[0x0020] = 0x20; //Space
    codepage_windows1250[0x0021] = 0x21; //Exclamation Mark
    codepage_windows1250[0x0022] = 0x22; //Quotation Mark
    codepage_windows1250[0x0023] = 0x23; //Number Sign
    codepage_windows1250[0x0024] = 0x24; //Dollar Sign
    codepage_windows1250[0x0025] = 0x25; //Percent Sign
    codepage_windows1250[0x0026] = 0x26; //Ampersand
    codepage_windows1250[0x0027] = 0x27; //Apostrophe
    codepage_windows1250[0x0028] = 0x28; //Left Parenthesis
    codepage_windows1250[0x0029] = 0x29; //Right Parenthesis
    codepage_windows1250[0x002a] = 0x2a; //Asterisk
    codepage_windows1250[0x002b] = 0x2b; //Plus Sign
    codepage_windows1250[0x002c] = 0x2c; //Comma
    codepage_windows1250[0x002d] = 0x2d; //Hyphen-Minus
    codepage_windows1250[0x002e] = 0x2e; //Full Stop
    codepage_windows1250[0x002f] = 0x2f; //Solidus
    codepage_windows1250[0x0030] = 0x30; //Digit Zero
    codepage_windows1250[0x0031] = 0x31; //Digit One
    codepage_windows1250[0x0032] = 0x32; //Digit Two
    codepage_windows1250[0x0033] = 0x33; //Digit Three
    codepage_windows1250[0x0034] = 0x34; //Digit Four
    codepage_windows1250[0x0035] = 0x35; //Digit Five
    codepage_windows1250[0x0036] = 0x36; //Digit Six
    codepage_windows1250[0x0037] = 0x37; //Digit Seven
    codepage_windows1250[0x0038] = 0x38; //Digit Eight
    codepage_windows1250[0x0039] = 0x39; //Digit Nine
    codepage_windows1250[0x003a] = 0x3a; //Colon
    codepage_windows1250[0x003b] = 0x3b; //Semicolon
    codepage_windows1250[0x003c] = 0x3c; //Less-Than Sign
    codepage_windows1250[0x003d] = 0x3d; //Equals Sign
    codepage_windows1250[0x003e] = 0x3e; //Greater-Than Sign
    codepage_windows1250[0x003f] = 0x3f; //Question Mark
    codepage_windows1250[0x0040] = 0x40; //Commercial At
    codepage_windows1250[0x0041] = 0x41; //Latin Capital Letter A
    codepage_windows1250[0x0042] = 0x42; //Latin Capital Letter B
    codepage_windows1250[0x0043] = 0x43; //Latin Capital Letter C
    codepage_windows1250[0x0044] = 0x44; //Latin Capital Letter D
    codepage_windows1250[0x0045] = 0x45; //Latin Capital Letter E
    codepage_windows1250[0x0046] = 0x46; //Latin Capital Letter F
    codepage_windows1250[0x0047] = 0x47; //Latin Capital Letter G
    codepage_windows1250[0x0048] = 0x48; //Latin Capital Letter H
    codepage_windows1250[0x0049] = 0x49; //Latin Capital Letter I
    codepage_windows1250[0x004a] = 0x4a; //Latin Capital Letter J
    codepage_windows1250[0x004b] = 0x4b; //Latin Capital Letter K
    codepage_windows1250[0x004c] = 0x4c; //Latin Capital Letter L
    codepage_windows1250[0x004d] = 0x4d; //Latin Capital Letter M
    codepage_windows1250[0x004e] = 0x4e; //Latin Capital Letter N
    codepage_windows1250[0x004f] = 0x4f; //Latin Capital Letter O
    codepage_windows1250[0x0050] = 0x50; //Latin Capital Letter P
    codepage_windows1250[0x0051] = 0x51; //Latin Capital Letter Q
    codepage_windows1250[0x0052] = 0x52; //Latin Capital Letter R
    codepage_windows1250[0x0053] = 0x53; //Latin Capital Letter S
    codepage_windows1250[0x0054] = 0x54; //Latin Capital Letter T
    codepage_windows1250[0x0055] = 0x55; //Latin Capital Letter U
    codepage_windows1250[0x0056] = 0x56; //Latin Capital Letter V
    codepage_windows1250[0x0057] = 0x57; //Latin Capital Letter W
    codepage_windows1250[0x0058] = 0x58; //Latin Capital Letter X
    codepage_windows1250[0x0059] = 0x59; //Latin Capital Letter Y
    codepage_windows1250[0x005a] = 0x5a; //Latin Capital Letter Z
    codepage_windows1250[0x005b] = 0x5b; //Left Square Bracket
    codepage_windows1250[0x005c] = 0x5c; //Reverse Solidus
    codepage_windows1250[0x005d] = 0x5d; //Right Square Bracket
    codepage_windows1250[0x005e] = 0x5e; //Circumflex Accent
    codepage_windows1250[0x005f] = 0x5f; //Low Line
    codepage_windows1250[0x0060] = 0x60; //Grave Accent
    codepage_windows1250[0x0061] = 0x61; //Latin Small Letter A
    codepage_windows1250[0x0062] = 0x62; //Latin Small Letter B
    codepage_windows1250[0x0063] = 0x63; //Latin Small Letter C
    codepage_windows1250[0x0064] = 0x64; //Latin Small Letter D
    codepage_windows1250[0x0065] = 0x65; //Latin Small Letter E
    codepage_windows1250[0x0066] = 0x66; //Latin Small Letter F
    codepage_windows1250[0x0067] = 0x67; //Latin Small Letter G
    codepage_windows1250[0x0068] = 0x68; //Latin Small Letter H
    codepage_windows1250[0x0069] = 0x69; //Latin Small Letter I
    codepage_windows1250[0x006a] = 0x6a; //Latin Small Letter J
    codepage_windows1250[0x006b] = 0x6b; //Latin Small Letter K
    codepage_windows1250[0x006c] = 0x6c; //Latin Small Letter L
    codepage_windows1250[0x006d] = 0x6d; //Latin Small Letter M
    codepage_windows1250[0x006e] = 0x6e; //Latin Small Letter N
    codepage_windows1250[0x006f] = 0x6f; //Latin Small Letter O
    codepage_windows1250[0x0070] = 0x70; //Latin Small Letter P
    codepage_windows1250[0x0071] = 0x71; //Latin Small Letter Q
    codepage_windows1250[0x0072] = 0x72; //Latin Small Letter R
    codepage_windows1250[0x0073] = 0x73; //Latin Small Letter S
    codepage_windows1250[0x0074] = 0x74; //Latin Small Letter T
    codepage_windows1250[0x0075] = 0x75; //Latin Small Letter U
    codepage_windows1250[0x0076] = 0x76; //Latin Small Letter V
    codepage_windows1250[0x0077] = 0x77; //Latin Small Letter W
    codepage_windows1250[0x0078] = 0x78; //Latin Small Letter X
    codepage_windows1250[0x0079] = 0x79; //Latin Small Letter Y
    codepage_windows1250[0x007a] = 0x7a; //Latin Small Letter Z
    codepage_windows1250[0x007b] = 0x7b; //Left Curly Bracket
    codepage_windows1250[0x007c] = 0x7c; //Vertical Line
    codepage_windows1250[0x007d] = 0x7d; //Right Curly Bracket
    codepage_windows1250[0x007e] = 0x7e; //Tilde
    codepage_windows1250[0x007f] = 0x7f; //Delete
    codepage_windows1250[0x20ac] = 0x80; //Euro Sign
    codepage_windows1250[0x0081] = 0x81; //    [0x82] = 0x201a; //Single Low-9 Quotation Mark
    codepage_windows1250[0x0083] = 0x83; //    [0x84] = 0x201e; //Double Low-9 Quotation Mark
    codepage_windows1250[0x2026] = 0x85; //Horizontal Ellipsis
    codepage_windows1250[0x2020] = 0x86; //Dagger
    codepage_windows1250[0x2021] = 0x87; //Double Dagger
    codepage_windows1250[0x0088] = 0x88; //    [0x89] = 0x2030; //Per Mille Sign
    codepage_windows1250[0x0160] = 0x8a; //Latin Capital Letter S With Caron
    codepage_windows1250[0x2039] = 0x8b; //Single Left-Pointing Angle Quotation Mark
    codepage_windows1250[0x015a] = 0x8c; //Latin Capital Letter S With Acute
    codepage_windows1250[0x0164] = 0x8d; //Latin Capital Letter T With Caron
    codepage_windows1250[0x017d] = 0x8e; //Latin Capital Letter Z With Caron
    codepage_windows1250[0x0179] = 0x8f; //Latin Capital Letter Z With Acute
    codepage_windows1250[0x0090] = 0x90; //    [0x91] = 0x2018; //Left Single Quotation Mark
    codepage_windows1250[0x2019] = 0x92; //Right Single Quotation Mark
    codepage_windows1250[0x201c] = 0x93; //Left Double Quotation Mark
    codepage_windows1250[0x201d] = 0x94; //Right Double Quotation Mark
    codepage_windows1250[0x2022] = 0x95; //Bullet
    codepage_windows1250[0x2013] = 0x96; //En Dash
    codepage_windows1250[0x2014] = 0x97; //Em Dash
    codepage_windows1250[0x0098] = 0x98; //    [0x99] = 0x2122; //Trade Mark Sign
    codepage_windows1250[0x0161] = 0x9a; //Latin Small Letter S With Caron
    codepage_windows1250[0x203a] = 0x9b; //Single Right-Pointing Angle Quotation Mark
    codepage_windows1250[0x015b] = 0x9c; //Latin Small Letter S With Acute
    codepage_windows1250[0x0165] = 0x9d; //Latin Small Letter T With Caron
    codepage_windows1250[0x017e] = 0x9e; //Latin Small Letter Z With Caron
    codepage_windows1250[0x017a] = 0x9f; //Latin Small Letter Z With Acute
    codepage_windows1250[0x00a0] = 0xa0; //No-Break Space
    codepage_windows1250[0x02c7] = 0xa1; //Caron
    codepage_windows1250[0x02d8] = 0xa2; //Breve
    codepage_windows1250[0x0141] = 0xa3; //Latin Capital Letter L With Stroke
    codepage_windows1250[0x00a4] = 0xa4; //Currency Sign
    codepage_windows1250[0x0104] = 0xa5; //Latin Capital Letter A With Ogonek
    codepage_windows1250[0x00a6] = 0xa6; //Broken Bar
    codepage_windows1250[0x00a7] = 0xa7; //Section Sign
    codepage_windows1250[0x00a8] = 0xa8; //Diaeresis
    codepage_windows1250[0x00a9] = 0xa9; //Copyright Sign
    codepage_windows1250[0x015e] = 0xaa; //Latin Capital Letter S With Cedilla
    codepage_windows1250[0x00ab] = 0xab; //Left-Pointing Double Angle Quotation Mark
    codepage_windows1250[0x00ac] = 0xac; //Not Sign
    codepage_windows1250[0x00ad] = 0xad; //Soft Hyphen
    codepage_windows1250[0x00ae] = 0xae; //Registered Sign
    codepage_windows1250[0x017b] = 0xaf; //Latin Capital Letter Z With Dot Above
    codepage_windows1250[0x00b0] = 0xb0; //Degree Sign
    codepage_windows1250[0x00b1] = 0xb1; //Plus-Minus Sign
    codepage_windows1250[0x02db] = 0xb2; //Ogonek
    codepage_windows1250[0x0142] = 0xb3; //Latin Small Letter L With Stroke
    codepage_windows1250[0x00b4] = 0xb4; //Acute Accent
    codepage_windows1250[0x00b5] = 0xb5; //Micro Sign
    codepage_windows1250[0x00b6] = 0xb6; //Pilcrow Sign
    codepage_windows1250[0x00b7] = 0xb7; //Middle Dot
    codepage_windows1250[0x00b8] = 0xb8; //Cedilla
    codepage_windows1250[0x0105] = 0xb9; //Latin Small Letter A With Ogonek
    codepage_windows1250[0x015f] = 0xba; //Latin Small Letter S With Cedilla
    codepage_windows1250[0x00bb] = 0xbb; //Right-Pointing Double Angle Quotation Mark
    codepage_windows1250[0x013d] = 0xbc; //Latin Capital Letter L With Caron
    codepage_windows1250[0x02dd] = 0xbd; //Double Acute Accent
    codepage_windows1250[0x013e] = 0xbe; //Latin Small Letter L With Caron
    codepage_windows1250[0x017c] = 0xbf; //Latin Small Letter Z With Dot Above
    codepage_windows1250[0x0154] = 0xc0; //Latin Capital Letter R With Acute
    codepage_windows1250[0x00c1] = 0xc1; //Latin Capital Letter A With Acute
    codepage_windows1250[0x00c2] = 0xc2; //Latin Capital Letter A With Circumflex
    codepage_windows1250[0x0102] = 0xc3; //Latin Capital Letter A With Breve
    codepage_windows1250[0x00c4] = 0xc4; //Latin Capital Letter A With Diaeresis
    codepage_windows1250[0x0139] = 0xc5; //Latin Capital Letter L With Acute
    codepage_windows1250[0x0106] = 0xc6; //Latin Capital Letter C With Acute
    codepage_windows1250[0x00c7] = 0xc7; //Latin Capital Letter C With Cedilla
    codepage_windows1250[0x010c] = 0xc8; //Latin Capital Letter C With Caron
    codepage_windows1250[0x00c9] = 0xc9; //Latin Capital Letter E With Acute
    codepage_windows1250[0x0118] = 0xca; //Latin Capital Letter E With Ogonek
    codepage_windows1250[0x00cb] = 0xcb; //Latin Capital Letter E With Diaeresis
    codepage_windows1250[0x011a] = 0xcc; //Latin Capital Letter E With Caron
    codepage_windows1250[0x00cd] = 0xcd; //Latin Capital Letter I With Acute
    codepage_windows1250[0x00ce] = 0xce; //Latin Capital Letter I With Circumflex
    codepage_windows1250[0x010e] = 0xcf; //Latin Capital Letter D With Caron
    codepage_windows1250[0x0110] = 0xd0; //Latin Capital Letter D With Stroke
    codepage_windows1250[0x0143] = 0xd1; //Latin Capital Letter N With Acute
    codepage_windows1250[0x0147] = 0xd2; //Latin Capital Letter N With Caron
    codepage_windows1250[0x00d3] = 0xd3; //Latin Capital Letter O With Acute
    codepage_windows1250[0x00d4] = 0xd4; //Latin Capital Letter O With Circumflex
    codepage_windows1250[0x0150] = 0xd5; //Latin Capital Letter O With Double Acute
    codepage_windows1250[0x00d6] = 0xd6; //Latin Capital Letter O With Diaeresis
    codepage_windows1250[0x00d7] = 0xd7; //Multiplication Sign
    codepage_windows1250[0x0158] = 0xd8; //Latin Capital Letter R With Caron
    codepage_windows1250[0x016e] = 0xd9; //Latin Capital Letter U With Ring Above
    codepage_windows1250[0x00da] = 0xda; //Latin Capital Letter U With Acute
    codepage_windows1250[0x0170] = 0xdb; //Latin Capital Letter U With Double Acute
    codepage_windows1250[0x00dc] = 0xdc; //Latin Capital Letter U With Diaeresis
    codepage_windows1250[0x00dd] = 0xdd; //Latin Capital Letter Y With Acute
    codepage_windows1250[0x0162] = 0xde; //Latin Capital Letter T With Cedilla
    codepage_windows1250[0x00df] = 0xdf; //Latin Small Letter Sharp S
    codepage_windows1250[0x0155] = 0xe0; //Latin Small Letter R With Acute
    codepage_windows1250[0x00e1] = 0xe1; //Latin Small Letter A With Acute
    codepage_windows1250[0x00e2] = 0xe2; //Latin Small Letter A With Circumflex
    codepage_windows1250[0x0103] = 0xe3; //Latin Small Letter A With Breve
    codepage_windows1250[0x00e4] = 0xe4; //Latin Small Letter A With Diaeresis
    codepage_windows1250[0x013a] = 0xe5; //Latin Small Letter L With Acute
    codepage_windows1250[0x0107] = 0xe6; //Latin Small Letter C With Acute
    codepage_windows1250[0x00e7] = 0xe7; //Latin Small Letter C With Cedilla
    codepage_windows1250[0x010d] = 0xe8; //Latin Small Letter C With Caron
    codepage_windows1250[0x00e9] = 0xe9; //Latin Small Letter E With Acute
    codepage_windows1250[0x0119] = 0xea; //Latin Small Letter E With Ogonek
    codepage_windows1250[0x00eb] = 0xeb; //Latin Small Letter E With Diaeresis
    codepage_windows1250[0x011b] = 0xec; //Latin Small Letter E With Caron
    codepage_windows1250[0x00ed] = 0xed; //Latin Small Letter I With Acute
    codepage_windows1250[0x00ee] = 0xee; //Latin Small Letter I With Circumflex
    codepage_windows1250[0x010f] = 0xef; //Latin Small Letter D With Caron
    codepage_windows1250[0x0111] = 0xf0; //Latin Small Letter D With Stroke
    codepage_windows1250[0x0144] = 0xf1; //Latin Small Letter N With Acute
    codepage_windows1250[0x0148] = 0xf2; //Latin Small Letter N With Caron
    codepage_windows1250[0x00f3] = 0xf3; //Latin Small Letter O With Acute
    codepage_windows1250[0x00f4] = 0xf4; //Latin Small Letter O With Circumflex
    codepage_windows1250[0x0151] = 0xf5; //Latin Small Letter O With Double Acute
    codepage_windows1250[0x00f6] = 0xf6; //Latin Small Letter O With Diaeresis
    codepage_windows1250[0x00f7] = 0xf7; //Division Sign
    codepage_windows1250[0x0159] = 0xf8; //Latin Small Letter R With Caron
    codepage_windows1250[0x016f] = 0xf9; //Latin Small Letter U With Ring Above
    codepage_windows1250[0x00fa] = 0xfa; //Latin Small Letter U With Acute
    codepage_windows1250[0x0171] = 0xfb; //Latin Small Letter U With Double Acute
    codepage_windows1250[0x00fc] = 0xfc; //Latin Small Letter U With Diaeresis
    codepage_windows1250[0x00fd] = 0xfd; //Latin Small Letter Y With Acute
    codepage_windows1250[0x0163] = 0xfe; //Latin Small Letter T With Cedilla
    codepage_windows1250[0x02d9] = 0xff; //Dot Above

    /// Windows 1251
    //Source: https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WindowsBestFit/bestfit1251.txt
    codepage_windows1251[0x0000] = 0x00; //Null
    codepage_windows1251[0x0001] = 0x01; //Start Of Heading
    codepage_windows1251[0x0002] = 0x02; //Start Of Text
    codepage_windows1251[0x0003] = 0x03; //End Of Text
    codepage_windows1251[0x0004] = 0x04; //End Of Transmission
    codepage_windows1251[0x0005] = 0x05; //Enquiry
    codepage_windows1251[0x0006] = 0x06; //Acknowledge
    codepage_windows1251[0x0007] = 0x07; //Bell
    codepage_windows1251[0x0008] = 0x08; //Backspace
    codepage_windows1251[0x0009] = 0x09; //Horizontal Tabulation
    codepage_windows1251[0x000a] = 0x0a; //Line Feed
    codepage_windows1251[0x000b] = 0x0b; //Vertical Tabulation
    codepage_windows1251[0x000c] = 0x0c; //Form Feed
    codepage_windows1251[0x000d] = 0x0d; //Carriage Return
    codepage_windows1251[0x000e] = 0x0e; //Shift Out
    codepage_windows1251[0x000f] = 0x0f; //Shift In
    codepage_windows1251[0x0010] = 0x10; //Data Link Escape
    codepage_windows1251[0x0011] = 0x11; //Device Control One
    codepage_windows1251[0x0012] = 0x12; //Device Control Two
    codepage_windows1251[0x0013] = 0x13; //Device Control Three
    codepage_windows1251[0x0014] = 0x14; //Device Control Four
    codepage_windows1251[0x0015] = 0x15; //Negative Acknowledge
    codepage_windows1251[0x0016] = 0x16; //Synchronous Idle
    codepage_windows1251[0x0017] = 0x17; //End Of Transmission Block
    codepage_windows1251[0x0018] = 0x18; //Cancel
    codepage_windows1251[0x0019] = 0x19; //End Of Medium
    codepage_windows1251[0x001a] = 0x1a; //Substitute
    codepage_windows1251[0x001b] = 0x1b; //Escape
    codepage_windows1251[0x001c] = 0x1c; //File Separator
    codepage_windows1251[0x001d] = 0x1d; //Group Separator
    codepage_windows1251[0x001e] = 0x1e; //Record Separator
    codepage_windows1251[0x001f] = 0x1f; //Unit Separator
    codepage_windows1251[0x0020] = 0x20; //Space
    codepage_windows1251[0x0021] = 0x21; //Exclamation Mark
    codepage_windows1251[0x0022] = 0x22; //Quotation Mark
    codepage_windows1251[0x0023] = 0x23; //Number Sign
    codepage_windows1251[0x0024] = 0x24; //Dollar Sign
    codepage_windows1251[0x0025] = 0x25; //Percent Sign
    codepage_windows1251[0x0026] = 0x26; //Ampersand
    codepage_windows1251[0x0027] = 0x27; //Apostrophe
    codepage_windows1251[0x0028] = 0x28; //Left Parenthesis
    codepage_windows1251[0x0029] = 0x29; //Right Parenthesis
    codepage_windows1251[0x002a] = 0x2a; //Asterisk
    codepage_windows1251[0x002b] = 0x2b; //Plus Sign
    codepage_windows1251[0x002c] = 0x2c; //Comma
    codepage_windows1251[0x002d] = 0x2d; //Hyphen-Minus
    codepage_windows1251[0x002e] = 0x2e; //Full Stop
    codepage_windows1251[0x002f] = 0x2f; //Solidus
    codepage_windows1251[0x0030] = 0x30; //Digit Zero
    codepage_windows1251[0x0031] = 0x31; //Digit One
    codepage_windows1251[0x0032] = 0x32; //Digit Two
    codepage_windows1251[0x0033] = 0x33; //Digit Three
    codepage_windows1251[0x0034] = 0x34; //Digit Four
    codepage_windows1251[0x0035] = 0x35; //Digit Five
    codepage_windows1251[0x0036] = 0x36; //Digit Six
    codepage_windows1251[0x0037] = 0x37; //Digit Seven
    codepage_windows1251[0x0038] = 0x38; //Digit Eight
    codepage_windows1251[0x0039] = 0x39; //Digit Nine
    codepage_windows1251[0x003a] = 0x3a; //Colon
    codepage_windows1251[0x003b] = 0x3b; //Semicolon
    codepage_windows1251[0x003c] = 0x3c; //Less-Than Sign
    codepage_windows1251[0x003d] = 0x3d; //Equals Sign
    codepage_windows1251[0x003e] = 0x3e; //Greater-Than Sign
    codepage_windows1251[0x003f] = 0x3f; //Question Mark
    codepage_windows1251[0x0040] = 0x40; //Commercial At
    codepage_windows1251[0x0041] = 0x41; //Latin Capital Letter A
    codepage_windows1251[0x0042] = 0x42; //Latin Capital Letter B
    codepage_windows1251[0x0043] = 0x43; //Latin Capital Letter C
    codepage_windows1251[0x0044] = 0x44; //Latin Capital Letter D
    codepage_windows1251[0x0045] = 0x45; //Latin Capital Letter E
    codepage_windows1251[0x0046] = 0x46; //Latin Capital Letter F
    codepage_windows1251[0x0047] = 0x47; //Latin Capital Letter G
    codepage_windows1251[0x0048] = 0x48; //Latin Capital Letter H
    codepage_windows1251[0x0049] = 0x49; //Latin Capital Letter I
    codepage_windows1251[0x004a] = 0x4a; //Latin Capital Letter J
    codepage_windows1251[0x004b] = 0x4b; //Latin Capital Letter K
    codepage_windows1251[0x004c] = 0x4c; //Latin Capital Letter L
    codepage_windows1251[0x004d] = 0x4d; //Latin Capital Letter M
    codepage_windows1251[0x004e] = 0x4e; //Latin Capital Letter N
    codepage_windows1251[0x004f] = 0x4f; //Latin Capital Letter O
    codepage_windows1251[0x0050] = 0x50; //Latin Capital Letter P
    codepage_windows1251[0x0051] = 0x51; //Latin Capital Letter Q
    codepage_windows1251[0x0052] = 0x52; //Latin Capital Letter R
    codepage_windows1251[0x0053] = 0x53; //Latin Capital Letter S
    codepage_windows1251[0x0054] = 0x54; //Latin Capital Letter T
    codepage_windows1251[0x0055] = 0x55; //Latin Capital Letter U
    codepage_windows1251[0x0056] = 0x56; //Latin Capital Letter V
    codepage_windows1251[0x0057] = 0x57; //Latin Capital Letter W
    codepage_windows1251[0x0058] = 0x58; //Latin Capital Letter X
    codepage_windows1251[0x0059] = 0x59; //Latin Capital Letter Y
    codepage_windows1251[0x005a] = 0x5a; //Latin Capital Letter Z
    codepage_windows1251[0x005b] = 0x5b; //Left Square Bracket
    codepage_windows1251[0x005c] = 0x5c; //Reverse Solidus
    codepage_windows1251[0x005d] = 0x5d; //Right Square Bracket
    codepage_windows1251[0x005e] = 0x5e; //Circumflex Accent
    codepage_windows1251[0x005f] = 0x5f; //Low Line
    codepage_windows1251[0x0060] = 0x60; //Grave Accent
    codepage_windows1251[0x0061] = 0x61; //Latin Small Letter A
    codepage_windows1251[0x0062] = 0x62; //Latin Small Letter B
    codepage_windows1251[0x0063] = 0x63; //Latin Small Letter C
    codepage_windows1251[0x0064] = 0x64; //Latin Small Letter D
    codepage_windows1251[0x0065] = 0x65; //Latin Small Letter E
    codepage_windows1251[0x0066] = 0x66; //Latin Small Letter F
    codepage_windows1251[0x0067] = 0x67; //Latin Small Letter G
    codepage_windows1251[0x0068] = 0x68; //Latin Small Letter H
    codepage_windows1251[0x0069] = 0x69; //Latin Small Letter I
    codepage_windows1251[0x006a] = 0x6a; //Latin Small Letter J
    codepage_windows1251[0x006b] = 0x6b; //Latin Small Letter K
    codepage_windows1251[0x006c] = 0x6c; //Latin Small Letter L
    codepage_windows1251[0x006d] = 0x6d; //Latin Small Letter M
    codepage_windows1251[0x006e] = 0x6e; //Latin Small Letter N
    codepage_windows1251[0x006f] = 0x6f; //Latin Small Letter O
    codepage_windows1251[0x0070] = 0x70; //Latin Small Letter P
    codepage_windows1251[0x0071] = 0x71; //Latin Small Letter Q
    codepage_windows1251[0x0072] = 0x72; //Latin Small Letter R
    codepage_windows1251[0x0073] = 0x73; //Latin Small Letter S
    codepage_windows1251[0x0074] = 0x74; //Latin Small Letter T
    codepage_windows1251[0x0075] = 0x75; //Latin Small Letter U
    codepage_windows1251[0x0076] = 0x76; //Latin Small Letter V
    codepage_windows1251[0x0077] = 0x77; //Latin Small Letter W
    codepage_windows1251[0x0078] = 0x78; //Latin Small Letter X
    codepage_windows1251[0x0079] = 0x79; //Latin Small Letter Y
    codepage_windows1251[0x007a] = 0x7a; //Latin Small Letter Z
    codepage_windows1251[0x007b] = 0x7b; //Left Curly Bracket
    codepage_windows1251[0x007c] = 0x7c; //Vertical Line
    codepage_windows1251[0x007d] = 0x7d; //Right Curly Bracket
    codepage_windows1251[0x007e] = 0x7e; //Tilde
    codepage_windows1251[0x007f] = 0x7f; //Delete
    codepage_windows1251[0x0402] = 0x80; //Cyrillic Capital Letter Dje
    codepage_windows1251[0x0403] = 0x81; //Cyrillic Capital Letter Gje
    codepage_windows1251[0x201a] = 0x82; //Single Low-9 Quotation Mark
    codepage_windows1251[0x0453] = 0x83; //Cyrillic Small Letter Gje
    codepage_windows1251[0x201e] = 0x84; //Double Low-9 Quotation Mark
    codepage_windows1251[0x2026] = 0x85; //Horizontal Ellipsis
    codepage_windows1251[0x2020] = 0x86; //Dagger
    codepage_windows1251[0x2021] = 0x87; //Double Dagger
    codepage_windows1251[0x20ac] = 0x88; //Euro Sign
    codepage_windows1251[0x2030] = 0x89; //Per Mille Sign
    codepage_windows1251[0x0409] = 0x8a; //Cyrillic Capital Letter Lje
    codepage_windows1251[0x2039] = 0x8b; //Single Left-Pointing Angle Quotation Mark
    codepage_windows1251[0x040a] = 0x8c; //Cyrillic Capital Letter Nje
    codepage_windows1251[0x040c] = 0x8d; //Cyrillic Capital Letter Kje
    codepage_windows1251[0x040b] = 0x8e; //Cyrillic Capital Letter Tshe
    codepage_windows1251[0x040f] = 0x8f; //Cyrillic Capital Letter Dzhe
    codepage_windows1251[0x0452] = 0x90; //Cyrillic Small Letter Dje
    codepage_windows1251[0x2018] = 0x91; //Left Single Quotation Mark
    codepage_windows1251[0x2019] = 0x92; //Right Single Quotation Mark
    codepage_windows1251[0x201c] = 0x93; //Left Double Quotation Mark
    codepage_windows1251[0x201d] = 0x94; //Right Double Quotation Mark
    codepage_windows1251[0x2022] = 0x95; //Bullet
    codepage_windows1251[0x2013] = 0x96; //En Dash
    codepage_windows1251[0x2014] = 0x97; //Em Dash
    codepage_windows1251[0x0098] = 0x98; //codepage_windows1251[0x2122] = 0x99; //Trade Mark Sign
    codepage_windows1251[0x0459] = 0x9a; //Cyrillic Small Letter Lje
    codepage_windows1251[0x203a] = 0x9b; //Single Right-Pointing Angle Quotation Mark
    codepage_windows1251[0x045a] = 0x9c; //Cyrillic Small Letter Nje
    codepage_windows1251[0x045c] = 0x9d; //Cyrillic Small Letter Kje
    codepage_windows1251[0x045b] = 0x9e; //Cyrillic Small Letter Tshe
    codepage_windows1251[0x045f] = 0x9f; //Cyrillic Small Letter Dzhe
    codepage_windows1251[0x00a0] = 0xa0; //No-Break Space
    codepage_windows1251[0x040e] = 0xa1; //Cyrillic Capital Letter Short U
    codepage_windows1251[0x045e] = 0xa2; //Cyrillic Small Letter Short U
    codepage_windows1251[0x0408] = 0xa3; //Cyrillic Capital Letter Je
    codepage_windows1251[0x00a4] = 0xa4; //Currency Sign
    codepage_windows1251[0x0490] = 0xa5; //Cyrillic Capital Letter Ghe With Upturn
    codepage_windows1251[0x00a6] = 0xa6; //Broken Bar
    codepage_windows1251[0x00a7] = 0xa7; //Section Sign
    codepage_windows1251[0x0401] = 0xa8; //Cyrillic Capital Letter Io
    codepage_windows1251[0x00a9] = 0xa9; //Copyright Sign
    codepage_windows1251[0x0404] = 0xaa; //Cyrillic Capital Letter Ukrainian Ie
    codepage_windows1251[0x00ab] = 0xab; //Left-Pointing Double Angle Quotation Mark
    codepage_windows1251[0x00ac] = 0xac; //Not Sign
    codepage_windows1251[0x00ad] = 0xad; //Soft Hyphen
    codepage_windows1251[0x00ae] = 0xae; //Registered Sign
    codepage_windows1251[0x0407] = 0xaf; //Cyrillic Capital Letter Yi
    codepage_windows1251[0x00b0] = 0xb0; //Degree Sign
    codepage_windows1251[0x00b1] = 0xb1; //Plus-Minus Sign
    codepage_windows1251[0x0406] = 0xb2; //Cyrillic Capital Letter Byelorussian-Ukrainian I
    codepage_windows1251[0x0456] = 0xb3; //Cyrillic Small Letter Byelorussian-Ukrainian I
    codepage_windows1251[0x0491] = 0xb4; //Cyrillic Small Letter Ghe With Upturn
    codepage_windows1251[0x00b5] = 0xb5; //Micro Sign
    codepage_windows1251[0x00b6] = 0xb6; //Pilcrow Sign
    codepage_windows1251[0x00b7] = 0xb7; //Middle Dot
    codepage_windows1251[0x0451] = 0xb8; //Cyrillic Small Letter Io
    codepage_windows1251[0x2116] = 0xb9; //Numero Sign
    codepage_windows1251[0x0454] = 0xba; //Cyrillic Small Letter Ukrainian Ie
    codepage_windows1251[0x00bb] = 0xbb; //Right-Pointing Double Angle Quotation Mark
    codepage_windows1251[0x0458] = 0xbc; //Cyrillic Small Letter Je
    codepage_windows1251[0x0405] = 0xbd; //Cyrillic Capital Letter Dze
    codepage_windows1251[0x0455] = 0xbe; //Cyrillic Small Letter Dze
    codepage_windows1251[0x0457] = 0xbf; //Cyrillic Small Letter Yi
    codepage_windows1251[0x0410] = 0xc0; //Cyrillic Capital Letter A
    codepage_windows1251[0x0411] = 0xc1; //Cyrillic Capital Letter Be
    codepage_windows1251[0x0412] = 0xc2; //Cyrillic Capital Letter Ve
    codepage_windows1251[0x0413] = 0xc3; //Cyrillic Capital Letter Ghe
    codepage_windows1251[0x0414] = 0xc4; //Cyrillic Capital Letter De
    codepage_windows1251[0x0415] = 0xc5; //Cyrillic Capital Letter Ie
    codepage_windows1251[0x0416] = 0xc6; //Cyrillic Capital Letter Zhe
    codepage_windows1251[0x0417] = 0xc7; //Cyrillic Capital Letter Ze
    codepage_windows1251[0x0418] = 0xc8; //Cyrillic Capital Letter I
    codepage_windows1251[0x0419] = 0xc9; //Cyrillic Capital Letter Short I
    codepage_windows1251[0x041a] = 0xca; //Cyrillic Capital Letter Ka
    codepage_windows1251[0x041b] = 0xcb; //Cyrillic Capital Letter El
    codepage_windows1251[0x041c] = 0xcc; //Cyrillic Capital Letter Em
    codepage_windows1251[0x041d] = 0xcd; //Cyrillic Capital Letter En
    codepage_windows1251[0x041e] = 0xce; //Cyrillic Capital Letter O
    codepage_windows1251[0x041f] = 0xcf; //Cyrillic Capital Letter Pe
    codepage_windows1251[0x0420] = 0xd0; //Cyrillic Capital Letter Er
    codepage_windows1251[0x0421] = 0xd1; //Cyrillic Capital Letter Es
    codepage_windows1251[0x0422] = 0xd2; //Cyrillic Capital Letter Te
    codepage_windows1251[0x0423] = 0xd3; //Cyrillic Capital Letter U
    codepage_windows1251[0x0424] = 0xd4; //Cyrillic Capital Letter Ef
    codepage_windows1251[0x0425] = 0xd5; //Cyrillic Capital Letter Ha
    codepage_windows1251[0x0426] = 0xd6; //Cyrillic Capital Letter Tse
    codepage_windows1251[0x0427] = 0xd7; //Cyrillic Capital Letter Che
    codepage_windows1251[0x0428] = 0xd8; //Cyrillic Capital Letter Sha
    codepage_windows1251[0x0429] = 0xd9; //Cyrillic Capital Letter Shcha
    codepage_windows1251[0x042a] = 0xda; //Cyrillic Capital Letter Hard Sign
    codepage_windows1251[0x042b] = 0xdb; //Cyrillic Capital Letter Yeru
    codepage_windows1251[0x042c] = 0xdc; //Cyrillic Capital Letter Soft Sign
    codepage_windows1251[0x042d] = 0xdd; //Cyrillic Capital Letter E
    codepage_windows1251[0x042e] = 0xde; //Cyrillic Capital Letter Yu
    codepage_windows1251[0x042f] = 0xdf; //Cyrillic Capital Letter Ya
    codepage_windows1251[0x0430] = 0xe0; //Cyrillic Small Letter A
    codepage_windows1251[0x0431] = 0xe1; //Cyrillic Small Letter Be
    codepage_windows1251[0x0432] = 0xe2; //Cyrillic Small Letter Ve
    codepage_windows1251[0x0433] = 0xe3; //Cyrillic Small Letter Ghe
    codepage_windows1251[0x0434] = 0xe4; //Cyrillic Small Letter De
    codepage_windows1251[0x0435] = 0xe5; //Cyrillic Small Letter Ie
    codepage_windows1251[0x0436] = 0xe6; //Cyrillic Small Letter Zhe
    codepage_windows1251[0x0437] = 0xe7; //Cyrillic Small Letter Ze
    codepage_windows1251[0x0438] = 0xe8; //Cyrillic Small Letter I
    codepage_windows1251[0x0439] = 0xe9; //Cyrillic Small Letter Short I
    codepage_windows1251[0x043a] = 0xea; //Cyrillic Small Letter Ka
    codepage_windows1251[0x043b] = 0xeb; //Cyrillic Small Letter El
    codepage_windows1251[0x043c] = 0xec; //Cyrillic Small Letter Em
    codepage_windows1251[0x043d] = 0xed; //Cyrillic Small Letter En
    codepage_windows1251[0x043e] = 0xee; //Cyrillic Small Letter O
    codepage_windows1251[0x043f] = 0xef; //Cyrillic Small Letter Pe
    codepage_windows1251[0x0440] = 0xf0; //Cyrillic Small Letter Er
    codepage_windows1251[0x0441] = 0xf1; //Cyrillic Small Letter Es
    codepage_windows1251[0x0442] = 0xf2; //Cyrillic Small Letter Te
    codepage_windows1251[0x0443] = 0xf3; //Cyrillic Small Letter U
    codepage_windows1251[0x0444] = 0xf4; //Cyrillic Small Letter Ef
    codepage_windows1251[0x0445] = 0xf5; //Cyrillic Small Letter Ha
    codepage_windows1251[0x0446] = 0xf6; //Cyrillic Small Letter Tse
    codepage_windows1251[0x0447] = 0xf7; //Cyrillic Small Letter Che
    codepage_windows1251[0x0448] = 0xf8; //Cyrillic Small Letter Sha
    codepage_windows1251[0x0449] = 0xf9; //Cyrillic Small Letter Shcha
    codepage_windows1251[0x044a] = 0xfa; //Cyrillic Small Letter Hard Sign
    codepage_windows1251[0x044b] = 0xfb; //Cyrillic Small Letter Yeru
    codepage_windows1251[0x044c] = 0xfc; //Cyrillic Small Letter Soft Sign
    codepage_windows1251[0x044d] = 0xfd; //Cyrillic Small Letter E
    codepage_windows1251[0x044e] = 0xfe; //Cyrillic Small Letter Yu
    codepage_windows1251[0x044f] = 0xff; //Cyrillic Small Letter Ya
    
    //Load codepages to UTF-8
    loadUTFCodepage(codepage_windows1250, 1250);
    loadUTFCodepage(codepage_windows1251, 1251);
}
