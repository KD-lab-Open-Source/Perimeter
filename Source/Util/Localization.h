#ifndef PERIMETER_LOCALIZATION_H
#define PERIMETER_LOCALIZATION_H

struct LocalizedText {
    std::string text = "";
    std::string locale = "";

    LocalizedText() = default;
    explicit LocalizedText(const std::string& text) : text(text) {}
    explicit LocalizedText(const std::string& text, const std::string& locale) : text(text), locale(locale) {}
    
    void set(const std::string& text_, const std::string& locale_) {
        text = text_;
        locale = locale_;
    }
};

void initLocale();
const std::string& getLocale();
const std::vector<std::string>& getLocales();
const char* getLocRootPath();
const std::string& getLocDataPath();

#endif //PERIMETER_LOCALIZATION_H
