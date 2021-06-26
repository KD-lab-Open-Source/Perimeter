SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")
CMAKE_MINIMUM_REQUIRED(VERSION 3.16.0)

# root
PROJECT(perimeter)

# Compiler / Host detections
IF(MSVC OR ${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
    SET(MSVC_CL_BUILD ON)
ENDIF()

# modules
IF(NOT MSVC_CL_BUILD)
	SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
ENDIF()

SET(CMAKE_CONFIGURATION_TYPES "Release;Debug;MinSizeRel;RelWithDebInfo")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS ON)

IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    SET(CLANG ON)
ELSE()
    SET(CLANG OFF)
ENDIF()

SET(PERIMETER_EXODUS OFF)
IF(CMAKE_HOST_WIN32 OR MINGW)
    MESSAGE("Windows or MinGW build")
    SET(PERIMETER_WINDOWS ON)
ELSE()
    MESSAGE("Not Windows or MinGW build - WARNING: EXPERIMENTAL!")
    SET(PERIMETER_WINDOWS OFF)
    SET(PERIMETER_EXODUS ON)
    SET(PERIMETER_EXODUS_SOUND ON)
ENDIF()

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE("Architecture: 64 bits - WARNING: EXPERIMENTAL!")
    SET(PERIMETER_ARCH_64 ON)
    ADD_DEFINITIONS(-DPERIMETER_ARCH_64)
ELSE()
    MESSAGE("Architecture: 32 bits")
    SET(PERIMETER_ARCH_64 OFF)
ENDIF()

# options
OPTION(OPTION_FINAL_VERSION "Generate Final version" ON)
OPTION(OPTION_STATIC_BUILD "Generate static build" OFF)
OPTION(OPTION_GEN_SCRIPTS "Re-Generates game scripts" OFF)
OPTION(OPTION_DISABLE_STACKTRACE "Disable stacktrace support" OFF)
OPTION(OPTION_LINKER_LLD "Use LLD linker in unix based compilers" ON)

# paths to search libs
IF(NOT MSVC_CL_BUILD)
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
    ")
ENDIF()

# depends
FIND_PACKAGE(Boost REQUIRED) # Boost stacktrace was introduced in 1.65 but is bugged
IF(MSVC_CL_BUILD)
    FIND_PACKAGE(Vorbis CONFIG REQUIRED)
    SET(VORBIS_FOUND TRUE)
    FIND_PACKAGE(SDL2 CONFIG REQUIRED)
ELSE()
    FIND_PACKAGE(Vorbis)
    FIND_PACKAGE(SDL2 REQUIRED)
ENDIF()

# globals
INCLUDE_DIRECTORIES(SYSTEM
    ${Boost_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIR}
)

IF(MSVC_CL_BUILD)
    INCLUDE_DIRECTORIES("${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/SDL2")
ELSE()
    IF(OPTION_STATIC_BUILD)
        SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        SET(BUILD_SHARED_LIBS OFF)
        SET(CMAKE_EXE_LINKER_FLAGS "-static")
    ENDIF()
    
    #Others
    INCLUDE(CheckCXXCompilerFlag)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=delete-non-virtual-dtor -Wno-conversion-null")
    IF(OPTION_LINKER_LLD)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=lld")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld")
    ENDIF()
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -ftree-vectorize -ffast-math")
    CHECK_CXX_COMPILER_FLAG("-msse2" HAVE_SIMDSSE)
    IF(HAVE_SIMDSSE)
        SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -msse2")
    ENDIF()

    # For modern gcc
    #add_definitions(-D_USE_MATH_DEFINES)

    #TODO remove these once code is more tidy
    add_compile_options(-Wno-misleading-indentation)
    add_compile_options(-Wno-unused-parameter)
    add_compile_options(-Wno-unused-function)
    add_compile_options(-Wno-unused-variable)
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
    add_compile_options(-Werror=shift-negative-value)
    add_compile_options(-Werror=dangling-else)
    add_compile_options(-Werror=parentheses)
    #add_compile_options(-Werror=write-strings) #TODO enable this again

    if (CLANG)
        add_compile_options(-Werror=mismatched-new-delete )
    endif()
ENDIF()

ADD_DEFINITIONS(-D_PERIMETER_)
#ADD_DEFINITIONS(-D_PERIMETER_ADDON_)
IF(OPTION_FINAL_VERSION)
    ADD_DEFINITIONS(-DNDEBUG)
    ADD_DEFINITIONS(-D_FINAL_VERSION_)
ELSE()
    MESSAGE("Not compiling Final version - WARNING: EXPERIMENTAL!")
ENDIF()
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    ADD_DEFINITIONS(-DPERIMETER_DEBUG)
    MESSAGE("Compiling Debug version")
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
