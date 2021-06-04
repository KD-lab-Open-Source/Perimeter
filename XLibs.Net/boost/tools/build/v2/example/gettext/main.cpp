
#include <locale.h>
#include <libintl.h>
#define i18n(s) gettext(s)

#include <iostream>
using namespace std;

int main()
{    
    // Specify that translations are stored in directory
    // "messages".
    bindtextdomain("main", "messages");
    textdomain("main");

    // Switch to russian locale.
    setlocale(LC_MESSAGES, "ru_RU.KOI8-R");

    // Output localized message.
    std::cout << i18n("hello") << "\n";

    return 0;
}
