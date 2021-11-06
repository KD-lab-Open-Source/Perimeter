#ifndef PERIMETER_LOCALIZATION_H
#define PERIMETER_LOCALIZATION_H

struct LocalizedText {
    std::string text = "";
    std::string locale = "";

    LocalizedText() = default;
    explicit LocalizedText(const std::string& text) : text(text) {}
    
    void set(const std::string& text_, const std::string& locale_) {
        text = text_;
        locale = locale_;
    }
};

void initLocale();
const std::string& getLocale();
const char* getLocRootPath();
const std::string& getLocDataPath();
char getLocaleChar(const char* utf8, const std::string& locale);
char getLocaleChar(const char* utf8);
std::string getLocaleString(const char* utf8, const std::string& locale);

#endif //PERIMETER_LOCALIZATION_H
