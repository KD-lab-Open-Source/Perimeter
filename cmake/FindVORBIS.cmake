# Taken from Vangers
# 
# Try to find Ogg/Vorbis libraries and include paths.
# Once done this will define
#
# VORBIS_FOUND
# VORBIS_INCLUDE_DIRS
# VORBIS_LIBRARIES
#

SET(MSVC_YEAR_NAME)
IF (MSVC_VERSION GREATER 1599)		# >= 1600
    SET(MSVC_YEAR_NAME VS2010)
ELSEIF(MSVC_VERSION GREATER 1499)	# >= 1500
    SET(MSVC_YEAR_NAME VS2008)
ELSEIF(MSVC_VERSION GREATER 1399)	# >= 1400
    SET(MSVC_YEAR_NAME VS2005)
ELSEIF(MSVC_VERSION GREATER 1299)	# >= 1300
    SET(MSVC_YEAR_NAME VS2003)
ELSEIF(MSVC_VERSION GREATER 1199)	# >= 1200
    SET(MSVC_YEAR_NAME VS6)
ENDIF()

SET(VORBIS_SEARCH_PATHS
    ${VORBIS_SEARCH_PATHS}
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
    /local/
)

FIND_PATH(OGG_INCLUDE_DIR
        NAMES ogg/ogg.h ogg/os_types.h
        HINTS
        $ENV{OGGDIR}
        $ENV{OGG_PATH}
        PATH_SUFFIXES include
        PATHS ${VORBIS_SEARCH_PATHS}
)

FIND_PATH(VORBIS_INCLUDE_DIR
        NAMES vorbis/codec.h
        HINTS
        $ENV{VORBISDIR}
        $ENV{VORBIS_PATH}
        PATH_SUFFIXES include
        PATHS ${VORBIS_SEARCH_PATHS}
)

FIND_PATH(VORBISFILE_INCLUDE_DIR
        NAMES vorbis/vorbisfile.h
        HINTS
        $ENV{VORBISFILEDIR}
        $ENV{VORBISFILE_PATH}
        $ENV{VORBISDIR}
        $ENV{VORBIS_PATH}
        PATH_SUFFIXES include
        PATHS ${VORBIS_SEARCH_PATHS}
)

FIND_LIBRARY(OGG_LIBRARY
        NAMES ogg libogg libogg libogg-0
        HINTS
        $ENV{VORBISDIR}
        $ENV{VORBIS_PATH}
        PATH_SUFFIXES lib lib64 win32/Ogg_Dynamic_Release "Win32/${MSVC_YEAR_NAME}/x64/Release" "Win32/${MSVC_YEAR_NAME}/Win32/Release"
        PATHS ${VORBIS_SEARCH_PATHS}
)

FIND_LIBRARY(VORBIS_LIBRARY
        NAMES vorbis libvorbis libvorbis-0
        HINTS
        $ENV{VORBISDIR}
        $ENV{VORBIS_PATH}
        PATH_SUFFIXES lib lib64 win32/Vorbis_Dynamic_Release "Win32/${MSVC_YEAR_NAME}/x64/Release" "Win32/${MSVC_YEAR_NAME}/Win32/Release"
        PATHS ${VORBIS_SEARCH_PATHS}
)

FIND_LIBRARY(VORBISFILE_LIBRARY
        NAMES vorbisfile libvorbisfile libvorbisfile-3
        HINTS
        $ENV{VORBISFILEDIR}
        $ENV{VORBISFILE_PATH}
        $ENV{VORBISDIR}
        $ENV{VORBIS_PATH}
        PATH_SUFFIXES lib lib64 win32/VorbisFile_Dynamic_Release "Win32/${MSVC_YEAR_NAME}/x64/Release" "Win32/${MSVC_YEAR_NAME}/Win32/Release"
        PATHS ${VORBIS_SEARCH_PATHS}
)

SET(VORBIS_LIBRARIES ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY} ${OGG_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(VORBIS DEFAULT_MSG VORBIS_LIBRARIES VORBIS_INCLUDE_DIR OGG_INCLUDE_DIR)

set(VORBIS_INCLUDE_DIRS ${OGG_INCLUDE_DIR} ${VORBIS_INCLUDE_DIR})

mark_as_advanced(OGG_INCLUDE_DIR VORBIS_INCLUDE_DIR OGG_LIBRARY VORBIS_LIBRARY VORBISFILE_LIBRARY)

