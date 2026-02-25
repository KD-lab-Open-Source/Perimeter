add_definitions(-DPERIMETER_INTEGRATION_STEAM)

add_library(IntegrationSteam STATIC 
    integration_steam.cpp
)

message("Steam SDK path: ${OPTION_INTEGRATION_STEAM_SDK}")

IF (MACOS)
    #Mac
    set(STEAM_LIB_PATH "osx/libsteam_api${CMAKE_SHARED_LIBRARY_SUFFIX}")
ELSEIF(PERIMETER_WINDOWS)
    #Windows
    IF(PERIMETER_ARCH_64)
        set(STEAM_LIB_PATH "win64/steam_api64${CMAKE_SHARED_LIBRARY_SUFFIX}")
    ELSE()
        set(STEAM_LIB_PATH "steam_api${CMAKE_SHARED_LIBRARY_SUFFIX}")
    ENDIF()
ELSE()
    #Linux
    IF(PERIMETER_ARCH_64)
        IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch")
            set(STEAM_LIB_PATH "linuxarm64/libsteam_api${CMAKE_SHARED_LIBRARY_SUFFIX}")
        ELSE()
            set(STEAM_LIB_PATH "linux64/libsteam_api${CMAKE_SHARED_LIBRARY_SUFFIX}")
        ENDIF ()
    ELSE()
        set(STEAM_LIB_PATH "linux32/libsteam_api${CMAKE_SHARED_LIBRARY_SUFFIX}")
    ENDIF()
ENDIF()

target_include_directories(IntegrationSteam PRIVATE
        .
        ..
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${OPTION_INTEGRATION_STEAM_SDK}/public/steam"
)

install (FILES "${OPTION_INTEGRATION_STEAM_SDK}/redistributable_bin/${STEAM_LIB_PATH}" TYPE LIB)

target_link_libraries(IntegrationSteam PRIVATE "${OPTION_INTEGRATION_STEAM_SDK}/redistributable_bin/${STEAM_LIB_PATH}")

target_compile_options(IntegrationSteam PRIVATE ${PERIMETER_COMPILE_OPTIONS})

target_link_libraries(Integrations PRIVATE IntegrationSteam)
