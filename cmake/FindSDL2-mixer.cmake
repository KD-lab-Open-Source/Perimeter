# - Find SDL2_mixer library and headers
# 
# Find module for SDL_mixer 2.0 (http://www.libsdl.org/projects/SDL_mixer/).
# It defines the following variables:
#  SDL2_MIXER_INCLUDE_DIRS - The location of the headers, e.g., SDL_mixer.h.
#  SDL2_MIXER_LIBRARIES - The libraries to link against to use SDL2_mixer.
#  SDL2_MIXER_FOUND - If false, do not try to use SDL2_mixer.
#  SDL2_MIXER_VERSION_STRING
#    Human-readable string containing the version of SDL2_mixer.
#
# Also defined, but not for general use are:
#   SDL2_MIXER_INCLUDE_DIR - The directory that contains SDL_mixer.h.
#   SDL2_MIXER_LIBRARY - The location of the SDL2_mixer library.
#

#=============================================================================
# Copyright 2013 Benjamin Eikel
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

find_package(PkgConfig QUIET)
pkg_check_modules(PC_SDL2_MIXER QUIET SDL2_mixer)

SET(SDL2_MIXER_SEARCH_PATHS
    ${SDL2_MIXER_SEARCH_PATHS}
    ${LIBRARY_SEARCH_PATHS}
)

find_path(SDL2_MIXER_INCLUDE_DIR
  NAMES SDL_mixer.h SDL-mixer.h
  HINTS
    ${PC_SDL2_MIXER_INCLUDEDIR}
    ${PC_SDL2_MIXER_INCLUDE_DIRS}
  PATH_SUFFIXES SDL2 SDL2_mixer SDL2-mixer
  PATHS
    ${SDL2_MIXER_SEARCH_PATHS}
)

find_library(SDL2_MIXER_LIBRARY
  NAMES SDL2_mixer SDL2-mixer
  HINTS
    ${PC_SDL2_MIXER_LIBDIR}
    ${PC_SDL2_MIXER_LIBRARY_DIRS}
  PATH_SUFFIXES x64 x86 lib lib64
  PATHS
    ${SDL2_MIXER_SEARCH_PATHS}
)

if(SDL2_MIXER_INCLUDE_DIR AND EXISTS "${SDL2_MIXER_INCLUDE_DIR}/SDL_mixer.h")
  file(STRINGS "${SDL2_MIXER_INCLUDE_DIR}/SDL_mixer.h" SDL2_MIXER_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL_MIXER_MAJOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL2_MIXER_INCLUDE_DIR}/SDL_mixer.h" SDL2_MIXER_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL_MIXER_MINOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL2_MIXER_INCLUDE_DIR}/SDL_mixer.h" SDL2_MIXER_VERSION_PATCH_LINE REGEX "^#define[ \t]+SDL_MIXER_PATCHLEVEL[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+SDL_MIXER_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_MIXER_VERSION_MAJOR "${SDL2_MIXER_VERSION_MAJOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL_MIXER_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_MIXER_VERSION_MINOR "${SDL2_MIXER_VERSION_MINOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL_MIXER_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" SDL2_MIXER_VERSION_PATCH "${SDL2_MIXER_VERSION_PATCH_LINE}")
  set(SDL2_MIXER_VERSION_STRING ${SDL2_MIXER_VERSION_MAJOR}.${SDL2_MIXER_VERSION_MINOR}.${SDL2_MIXER_VERSION_PATCH})
  unset(SDL2_MIXER_VERSION_MAJOR_LINE)
  unset(SDL2_MIXER_VERSION_MINOR_LINE)
  unset(SDL2_MIXER_VERSION_PATCH_LINE)
  unset(SDL2_MIXER_VERSION_MAJOR)
  unset(SDL2_MIXER_VERSION_MINOR)
  unset(SDL2_MIXER_VERSION_PATCH)
endif()

set(SDL2_MIXER_INCLUDE_DIRS ${SDL2_MIXER_INCLUDE_DIR})
set(SDL2_MIXER_LIBRARIES ${SDL2_MIXER_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(SDL2-mixer
                                  REQUIRED_VARS SDL2_MIXER_INCLUDE_DIRS SDL2_MIXER_LIBRARIES
                                  VERSION_VAR SDL2_MIXER_VERSION_STRING)

mark_as_advanced(SDL2_MIXER_INCLUDE_DIR SDL2_MIXER_LIBRARY)
