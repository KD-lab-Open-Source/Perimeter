CMAKE_MINIMUM_REQUIRED(VERSION 3.16.0)

# root
PROJECT(perimeter)

SET(CMAKE_CONFIGURATION_TYPES "Release;Debug;MinSizeRel;RelWithDebInfo")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS ON)
SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")

# Options and inputs
OPTION(OPTION_FINAL_VERSION "Generate Final version" ON)
OPTION(OPTION_STATIC_BUILD "Generate static build" OFF)
OPTION(OPTION_CHECK_SCRIPTS "Check game scripts with XPrm" OFF)
OPTION(OPTION_PROCESS_SCRIPTS "Re-Process game scripts with XPrm" OFF)
OPTION(OPTION_DISABLE_STACKTRACE "Disable stacktrace support" OFF)
OPTION(OPTION_LINKER_LLD "Use LLD linker in unix based compilers" ON)
OPTION(OPTION_DEBUG_ASSERT "Enable debug assertions" OFF)
OPTION(OPTION_GPERFTOOLS "Link executable with gperftools for debugging" OFF)
OPTION(OPTION_HEADLESS "Enable headless mode (no window or rendering)" OFF)
OPTION(OPTION_DXVK "Use dxvk-native for D3D9 API in Linux" ON)

# Compiler detections
SET(MSVC_CL_BUILD OFF)
SET(CLANG OFF)
IF(MSVC OR ${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
    SET(MSVC_CL_BUILD ON)
ENDIF()
IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    SET(CLANG ON)
ENDIF()

#Detect 32 or 64 bits compiler
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE("Architecture: 64 bits")
    SET(PERIMETER_ARCH_64 ON)
    ADD_DEFINITIONS(-DPERIMETER_ARCH_64)
ELSE()
    MESSAGE("Architecture: 32 bits")
    SET(PERIMETER_ARCH_64 OFF)
ENDIF()

# Host detections and tweaks
MESSAGE("CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
SET(MACOS OFF)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOS ON)
    set(OPTION_LINKER_LLD OFF)
endif()

SET(PERIMETER_EXE_FLAGS "")
SET(PERIMETER_EXODUS OFF)
IF(CMAKE_HOST_WIN32 OR MINGW)
    MESSAGE("Windows or MinGW build")
    SET(PERIMETER_WINDOWS ON)
    #Compile exe as Win32 App instead of Console App
    #Also add icon via .rc
    SET(PERIMETER_EXE_FLAGS WIN32 perimeter.rc)
    #Tells SDL that we handle main
    add_definitions(-DSDL_MAIN_HANDLED)
ELSE()
    MESSAGE("Not Windows or MinGW build - WARNING: EXPERIMENTAL!")
    SET(PERIMETER_WINDOWS OFF)
    SET(PERIMETER_EXODUS ON)
    SET(PERIMETER_EXODUS_SOUND ON)
ENDIF()

IF(OPTION_HEADLESS)
    MESSAGE("Compiling headless build")
    SET(OPTION_DXVK OFF)
    add_definitions(-DPERIMETER_HEADLESS)
ELSE()
    ADD_DEFINITIONS(-DPERIMETER_D3D9)
ENDIF()

#To allow downloading deps
INCLUDE(FetchContent)
INCLUDE(ExternalProject)

#Is a debug build?
SET(PERIMETER_DEBUG OFF)
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    SET(PERIMETER_DEBUG ON)
    #On debug mode don't download every time unless its absent
    SET(FETCHCONTENT_UPDATES_DISCONNECTED ON)
ENDIF()

# Dependencies
FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
IF(MSVC_CL_BUILD)
    #Specifics to MSVC+VCPKG
    FIND_PACKAGE(SDL2 CONFIG REQUIRED)
    SET(SDL2_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/SDL2")
    SET(SDL2_LIBRARY SDL2::SDL2) 
    SET(SDL2MAIN_LIBRARY SDL2::SDL2main)
    FIND_PACKAGE(SDL2-image CONFIG REQUIRED)
    SET(SDL2_IMAGE_LIBRARY SDL2::SDL2_image)
    FIND_PACKAGE(SDL2-mixer CONFIG REQUIRED)
    SET(SDL2_MIXER_LIBRARY SDL2::SDL2_mixer)
    FIND_PACKAGE(SDL2-net CONFIG REQUIRED)
    SET(SDL2_NET_LIBRARY SDL2::SDL2_net)
ELSE()
    # paths to search libs
    SET(LIBRARY_SEARCH_PATHS "
        ${LIBRARY_SEARCH_PATHS}
        ~/Library/Frameworks 
        /Library/Frameworks
        /mingw
        /usr/pkg
        /usr/local
        /usr
        /sw # Fink
        /opt/local # DarwinPorts
        /opt/csw # Blastwave
        /opt
        /local
        C:/MinGW
        /usr/include/x86_64-linux-gnu
        /usr/include/aarch64-linux-gnu
        /usr/lib/x86_64-linux-gnu
        /usr/lib/aarch64-linux-gnu
    ")
    
    SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
    
    #This makes SDL2 not include SDL2main
    SET(SDL2_BUILDING_LIBRARY ON)
    FIND_PACKAGE(SDL2 REQUIRED)
    FIND_PACKAGE(SDL2-image REQUIRED)
    FIND_PACKAGE(SDL2-mixer REQUIRED)
    FIND_PACKAGE(SDL2-net REQUIRED)
ENDIF()

#FFMPEG used for AVI and video decoding if present
IF (PERIMETER_WINDOWS)
    FIND_PACKAGE(FFMPEG)
ELSE()
    #Required since there is no alternative for loading AVI textures 
    FIND_PACKAGE(FFMPEG REQUIRED)
ENDIF()

# globals
INCLUDE_DIRECTORIES(SYSTEM
    ${Boost_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${SDL2_MIXER_INCLUDE_DIR}
    ${SDL2_NET_INCLUDE_DIR}
)
SET(PERIMETER_COMPILE_OPTIONS)

IF(FFMPEG_FOUND)
    #FFMPEG is found
    ADD_DEFINITIONS(-DPERIMETER_FFMPEG)
    INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIRS})
ENDIF()

#Required for more compilant floating point math
IF(MSVC_CL_BUILD)
    add_compile_options(/fp:precise)
ELSE()
    add_compile_options(-fno-fast-math -ffp-contract=off)
    
    #Arch specific flags
    IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64")
        #Enable SSE/SSE2 in x86 arch
        MESSAGE("CPU is x86 type, using SSE")
        add_compile_options(-mfpmath=sse -msse -msse2)
        IF (NOT PERIMETER_ARCH_64)
            #Might be required for SSE being enabled
            add_compile_options(-march=pentium4)
        ENDIF()
    ELSEIF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch")
        #ARM32/ARM64
        IF (NOT PERIMETER_ARCH_64)
            MESSAGE("CPU is ARM, using VFP WARNING: untested")
            add_compile_options(-mfpmath=vfp)
        ENDIF()
    ENDIF()
ENDIF()

IF(NOT MSVC_CL_BUILD)
    # Configure lib suffixes for MinGW unless is static build
    IF(MINGW AND NOT OPTION_STATIC_BUILD)
        SET(CMAKE_FIND_LIBRARY_SUFFIXES
            ".a"
            ".dll.a"
            ".a.lib"
        )
    ENDIF()
    IF(OPTION_STATIC_BUILD)
        SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        SET(BUILD_SHARED_LIBS OFF)
        SET(CMAKE_EXE_LINKER_FLAGS "-static")
    ENDIF()
    
    #Others
    IF(CLANG)
        add_compile_options(-Werror=unused-command-line-argument)
    ENDIF()
    INCLUDE(CheckCXXCompilerFlag)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder -Werror=delete-non-virtual-dtor -Wno-conversion-null")
    IF(OPTION_LINKER_LLD)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=lld")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld")
    ENDIF()
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
    IF(MACOS)
        SET(CMAKE_BUILD_RPATH "./dxvk-native-prefix/src/dxvk-native-build/src/d3d9/")
    ELSE()
        SET(CMAKE_BUILD_RPATH "$ORIGIN") #let binary find libs on own dir
        SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -no-pie -fno-pie")
    ENDIF()
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -ftree-vectorize")
    
    #TODO remove these once code is more tidy
    add_compile_options(-Wno-misleading-indentation)
    add_compile_options(-Wno-unused-parameter)
    add_compile_options(-Wno-unused-function)
    add_compile_options(-Wno-unused-variable)
    add_compile_options(-Wno-comment)
    IF(CLANG)
        add_compile_options(-Wno-missing-braces)
        add_compile_options(-Wno-invalid-source-encoding)
    ELSE()
        add_compile_options(-Wno-unused-but-set-variable)
    ENDIF()
    add_compile_options(-Wno-unused-value)
    add_compile_options(-Wno-deprecated)
    
    #TODO remove these once code is more reasonable
    add_compile_options(-Wno-sign-compare)
    add_compile_options(-Wno-narrowing)
    add_compile_options(-Wno-write-strings)
    add_compile_options(-Wno-return-type)
    IF(CLANG)
        add_compile_options(-Wno-reorder-ctor)
    ENDIF()

    add_compile_options(-Wno-unknown-pragmas) #TODO we should remove these pragmas?
    add_compile_options(-Werror=dangling-else)
    add_compile_options(-Werror=return-type)
    # add_compile_options(-Werror=switch) #TODO enable for strict check of switch block
    # add_compile_options(-Werror=write-strings) #TODO enable this again

    if (CLANG)
        add_compile_options(-Werror=mismatched-new-delete )
    endif()
    
    #These options may cause issues with external code compilation, so we apply only to our own targets
    SET(PERIMETER_COMPILE_OPTIONS -Werror=shift-negative-value)
    SET(PERIMETER_COMPILE_OPTIONS -Werror=parentheses)
ENDIF()

ADD_DEFINITIONS(-D_PERIMETER_)
IF(OPTION_FINAL_VERSION)
    ADD_DEFINITIONS(-DNDEBUG)
    ADD_DEFINITIONS(-D_FINAL_VERSION_)
ELSE()
    MESSAGE("Not compiling Final version - WARNING: EXPERIMENTAL!")
ENDIF()
IF(PERIMETER_DEBUG)
    ADD_DEFINITIONS(-DPERIMETER_DEBUG)
    MESSAGE("Compiling Debug version")
    IF(OPTION_DEBUG_ASSERT)
        ADD_DEFINITIONS(-DPERIMETER_DEBUG_ASSERT)
        MESSAGE("Debug assertions enabled")
    ENDIF()
ENDIF()
IF(OPTION_DISABLE_STACKTRACE)
    ADD_DEFINITIONS(-DOPTION_DISABLE_STACKTRACE)
ENDIF()
IF(PERIMETER_EXODUS)
    ADD_DEFINITIONS(-DPERIMETER_EXODUS)
ENDIF()
IF(PERIMETER_EXODUS_SOUND)
    ADD_DEFINITIONS(-DPERIMETER_EXODUS_SOUND)
ENDIF()

ADD_SUBDIRECTORY("Source")
