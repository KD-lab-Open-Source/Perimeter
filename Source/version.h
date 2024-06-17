#define VERSION "3.1.9"

//Sanity check to make sure the cmake version matches the code version
#include <string_view>
static_assert(
        std::string_view(PERIMETER_VERSION) == VERSION,
        "'" PERIMETER_VERSION "' mismatches the expected version '" VERSION "'"
);
