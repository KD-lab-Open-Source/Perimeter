#ifndef PERIMETER_CODEPAGES_H
#define PERIMETER_CODEPAGES_H

/*
 * Functions to convert between UTF8 -> codepage and viceversa
 * 
 * https://docs.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
 * 
 * NOTE: Russian language uses windows-1251 codepage
 * The rest of languages which game was released in Europe (English, Spanish, German, French and Italian)
 * uses windows-1250 codepage, 1252 would have been more correct for these but for now we use 1250
 */

std::string convertToCodepage(const char* utf8, const std::string& locale);
std::string convertToCodepage(const char* utf8, uint16_t codepage);
std::string convertToUnicode(const std::string& str, const std::string& locale);
std::string convertToUnicode(const std::string& str, uint16_t codepage);

#endif //PERIMETER_CODEPAGES_H
