
add_library(Exodus STATIC
        exodus.cpp
        ${pevents_SOURCE_DIR}/src/pevents.cpp
)

target_include_directories(Exodus PRIVATE
        .
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

target_compile_options(Exodus PRIVATE ${PERIMETER_COMPILE_OPTIONS})
