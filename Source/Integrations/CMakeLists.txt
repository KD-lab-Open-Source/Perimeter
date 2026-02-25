add_library(Integrations STATIC
    integrations.cpp
)

target_compile_options(Integrations PRIVATE ${PERIMETER_COMPILE_OPTIONS})

target_include_directories(Integrations PRIVATE .)

IF(OPTION_INTEGRATION_STEAM_SDK)
    ADD_SUBDIRECTORY("Steam")
ELSE()
    message("No Steam integration provided")
ENDIF()
