add_library(IntegrationSteam STATIC 
    integration_steam.cpp
)

message("Steam SDK path: ${OPTION_INTEGRATION_STEAM_SDK}")

IF (MACOS)
    #Mac
    set(STEAM_LIB_PATH "osx/libsteam_api${CMAKE_SHARED_LIBRARY_SUFFIX}")
ELSEIF(PERIMETER_WINDOWS)
    #Windows
    if (MSVC_CL_BUILD)
        #MSVC
        if(PERIMETER_ARCH_64)
            set(STEAM_LIB_PATH "win64/steam_api64${CMAKE_LINK_LIBRARY_SUFFIX}")
        else()
            set(STEAM_LIB_PATH "steam_api${CMAKE_LINK_LIBRARY_SUFFIX}")
        endif()
    else()
        #MSYS
        if(PERIMETER_ARCH_64)
            set(STEAM_LIB_PATH "win64/steam_api64${CMAKE_SHARED_LIBRARY_SUFFIX}")
        else()
            set(STEAM_LIB_PATH "steam_api${CMAKE_SHARED_LIBRARY_SUFFIX}")
        endif()
    endif()
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
        "${PROJECT_SOURCE_DIR}/Source/Units"
        "${PROJECT_SOURCE_DIR}/Source/Game"
        "${OPTION_INTEGRATION_STEAM_SDK}/public/steam"
)

install (FILES "${OPTION_INTEGRATION_STEAM_SDK}/redistributable_bin/${STEAM_LIB_PATH}" TYPE LIB)

target_link_libraries(IntegrationSteam PRIVATE "${OPTION_INTEGRATION_STEAM_SDK}/redistributable_bin/${STEAM_LIB_PATH}")

set(INTEGRATION_STEAM_COMPILE_OPTIONS ${PERIMETER_COMPILE_OPTIONS})

if (NOT MSVC_CL_BUILD)
    set(INTEGRATION_STEAM_COMPILE_OPTIONS 
        ${PERIMETER_COMPILE_OPTIONS}
        #STEAM_CALLBACK needs this
        -Wno-overloaded-virtual
    )
endif ()

target_compile_options(IntegrationSteam PRIVATE ${INTEGRATION_STEAM_COMPILE_OPTIONS})

target_link_libraries(Integrations PRIVATE IntegrationSteam)
