# Keep headers and libraries from the same pkg-config search on macOS.
# Independent framework/header searches can otherwise select GStreamer headers
# while linking against Homebrew FFmpeg libraries with a different ABI.
if(APPLE)
    set(FFMPEG_ROOT "$ENV{FFMPEG_DIR}" CACHE PATH "Location of FFMPEG")
    set(_ffmpeg_prefix_path "${CMAKE_PREFIX_PATH}")
    if(FFMPEG_ROOT)
        list(PREPEND CMAKE_PREFIX_PATH "${FFMPEG_ROOT}")
    endif()
    find_package(PkgConfig QUIET)
    set(_ffmpeg_modules libavformat libavcodec libavutil)
    if(OPTION_FFMPEG_MOVIES)
        list(APPEND _ffmpeg_modules libswscale libavfilter)
    endif()
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(FFMPEG QUIET IMPORTED_TARGET ${_ffmpeg_modules})
    else()
        set(FFMPEG_FOUND FALSE)
    endif()
    set(CMAKE_PREFIX_PATH "${_ffmpeg_prefix_path}")
    unset(_ffmpeg_prefix_path)
    unset(_ffmpeg_modules)

    # Clear component results left by the previous finder in an existing cache.
    set(FFMPEG_LIBRARIES "")
    set(FFMPEG_LIBSWSCALE_INCLUDE_DIRS "")
    set(FFMPEG_LIBAVFILTER_INCLUDE_DIRS "")
    set(FFMPEG_LIBSWSCALE_LIBRARIES "")
    set(FFMPEG_LIBAVFILTER_LIBRARIES "")
    set(FFMPEG_LIBSWRESAMPLE_FOUND FALSE)
    if(FFMPEG_FOUND)
        set(FFMPEG_LIBRARIES PkgConfig::FFMPEG)
    endif()
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(FFMPEG REQUIRED_VARS FFMPEG_FOUND FFMPEG_LIBRARIES)
    return()
endif()

# Locate ffmpeg
# This module defines
# FFMPEG_LIBRARIES
# FFMPEG_FOUND, if false, do not try to link to ffmpeg
# FFMPEG_INCLUDE_DIR, where to find the headers
#
# $FFMPEG_DIR is an environment variable that would
# correspond to the ./configure --prefix=$FFMPEG_DIR
#
# Created by Robert Osfield.
# Modified by Lukas Lalinsky.
# Modified by Ion Agorria for Perimeter specifics needs.


#In ffmpeg code, old version use "#include <header.h>" and newer use "#include <libname/header.h>"
#In OSG ffmpeg plugin, we use "#include <header.h>" for compatibility with old version of ffmpeg

#We have to search the path which contain the header.h (usefull for old version)
#and search the path which contain the libname/header.h (usefull for new version)

#Then we need to include ${FFMPEG_libname_INCLUDE_DIRS} (in old version case, use by ffmpeg header and osg plugin code)
#                                                       (in new version case, use by ffmpeg header) 
#and ${FFMPEG_libname_INCLUDE_DIRS/libname}             (in new version case, use by osg plugin code)

# Macro to find header and lib directories
# example: FFMPEG_FIND(AVFORMAT avformat avformat.h)
MACRO(FFMPEG_FIND varname shortname headername)

	FIND_PATH(FFMPEG_${varname}_INCLUDE_DIRS
			NAMES
			lib${shortname}/${headername}
			PATHS
			${FFMPEG_SEARCH_PATHS}
			PATH_SUFFIXES
			include
			include/ffmpeg
			ffmpeg
			DOC "Location of FFMPEG Headers"
			)
	
	FIND_LIBRARY(FFMPEG_${varname}_LIBRARIES
			NAMES
			${shortname}
			PATHS
			${FFMPEG_SEARCH_PATHS}
			PATH_SUFFIXES
			lib64
			lib
			lib64/ffmpeg
			lib/ffmpeg
			)

	IF (FFMPEG_${varname}_LIBRARIES AND FFMPEG_${varname}_INCLUDE_DIRS)
		SET(FFMPEG_${varname}_FOUND 1)
		MESSAGE("Found FFMPEG ${shortname}: ${FFMPEG_${varname}_INCLUDE_DIRS} ${FFMPEG_${varname}_LIBRARIES}")
	ENDIF()

ENDMACRO(FFMPEG_FIND)

SET(FFMPEG_ROOT "$ENV{FFMPEG_DIR}" CACHE PATH "Location of FFMPEG")

SET(FFMPEG_SEARCH_PATHS
		${FFMPEG_LIBRARIES}
		${FFMPEG_INCLUDE_DIRS}
		${FFMPEG_ROOT}/include
		$ENV{FFMPEG_DIR}/include
		/usr/pkg/include/ffmpeg3/
		/local/bin
		/bin
		${FFMPEG_SEARCH_PATHS}
		${LIBRARY_SEARCH_PATHS}
		)

FFMPEG_FIND(LIBAVFORMAT avformat avformat.h)
#FFMPEG_FIND(LIBAVDEVICE avdevice avdevice.h)
FFMPEG_FIND(LIBAVFILTER avfilter avfilter.h)
FFMPEG_FIND(LIBAVCODEC avcodec avcodec.h)
FFMPEG_FIND(LIBAVCODEC_FFT avcodec avfft.h)
FFMPEG_FIND(LIBAVUTIL avutil avutil.h)
FFMPEG_FIND(LIBSWSCALE swscale swscale.h)
FFMPEG_FIND(LIBSWRESAMPLE swresample swresample.h)

SET(FFMPEG_FOUND "NO")
# Note we don't check FFMPEG_LIBSWSCALE_FOUND or FFMPEG_LIBSWRESAMPLE_FOUND here, it's optional.
# We also don't need FFMPEG_LIBAVDEVICE_FOUND
IF (FFMPEG_LIBAVFORMAT_FOUND AND FFMPEG_LIBAVCODEC_FOUND AND FFMPEG_LIBAVUTIL_FOUND)
	SET(FFMPEG_INCLUDE_DIRS ${FFMPEG_LIBAVFORMAT_INCLUDE_DIRS})
	SET(FFMPEG_LIBRARY_DIRS ${FFMPEG_LIBAVFORMAT_LIBRARY_DIRS})
	SET(FFMPEG_LIBRARIES
			${FFMPEG_LIBAVFORMAT_LIBRARIES}
			${FFMPEG_LIBAVCODEC_LIBRARIES}
			${FFMPEG_LIBAVUTIL_LIBRARIES}
	)
ENDIF()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFMPEG REQUIRED_VARS FFMPEG_INCLUDE_DIRS FFMPEG_LIBRARIES)
