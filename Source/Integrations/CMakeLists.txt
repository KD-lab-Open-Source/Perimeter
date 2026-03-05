add_library(Integrations STATIC
    integrations.cpp
)

target_compile_options(Integrations PRIVATE ${PERIMETER_COMPILE_OPTIONS})

target_include_directories(Integrations PRIVATE
        .
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/Source/Units"
        "${PROJECT_SOURCE_DIR}/Source/Game"
        "${OPTION_INTEGRATION_STEAM_SDK}/public/steam"
)

IF(OPTION_INTEGRATION_STEAM_SDK)
    add_definitions(-DPERIMETER_INTEGRATION_STEAM)
    ADD_SUBDIRECTORY("Steam")
ELSE()
    message("No Steam integration provided")
ENDIF()
