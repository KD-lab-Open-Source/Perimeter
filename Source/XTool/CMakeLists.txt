
#GameMath retrieval
FetchContent_Declare(gamemath
    GIT_REPOSITORY    https://github.com/IonAgorria/gamemath
    GIT_TAG           origin/master
    GIT_SHALLOW       ON
)
FetchContent_MakeAvailable(gamemath)

#Workaround for some compilers that not recognize __always_inline
target_compile_definitions(gamemath_object_lib PUBLIC -D__always_inline=)
target_compile_definitions(gamemath_static_lib PUBLIC -D__always_inline=)

INCLUDE_DIRECTORIES(
    BEFORE .
    "${gamemath_SOURCE_DIR}/include"
)

SET(XTool_SRCS
    xmath/xmath.cpp
    xmath/gamemath.cpp
    xmath/std.cpp
    XBUFFER/XBCNVIN.CPP
    XBUFFER/XBCNVOUT.CPP
    XBUFFER/XBCORE.CPP
    XBUFFER/XBSEARCH.CPP
    XSTREAM/XSCNVOUT.CPP
    XSTREAM/XSSERV.CPP
    XSTREAM/XSRDWR.CPP
    XSTREAM/XSCNVIN.CPP
    XSTREAM/XSENLV.CPP
    XSTREAM/XSCORE.CPP
    xerrhand.cpp
    XUTIL/XUTIL.CPP
    XUTIL/XClock.cpp
    files/files.cpp
)

SET(XTool_LINK_LIBS gamemath_static_lib ZLIB::ZLIB)

IF(NOT MSVC_CL_BUILD AND NOT OPTION_DISABLE_STACKTRACE)
    IF(PERIMETER_WINDOWS)
        #Use Backtrace in MinGW/MSYS if available, else ADDR2LINE
        FIND_PACKAGE(Backtrace)
        IF(Backtrace_FOUND)
            ADD_DEFINITIONS(-DBOOST_STACKTRACE_USE_BACKTRACE)
            SET(XTool_LINK_LIBS ${XTool_LINK_LIBS} ${Backtrace_LIBRARIES})
        ELSE()
            ADD_DEFINITIONS(-DBOOST_STACKTRACE_USE_ADDR2LINE)
            # dl is required for dladdr in stacktrace
            SET(XTool_LINK_LIBS ${XTool_LINK_LIBS} dl)
        ENDIF()
    ELSE()
        #Use ADDR2LINE in POSIX
        ADD_DEFINITIONS(-DBOOST_STACKTRACE_USE_ADDR2LINE)
        # dl is required for dladdr in stacktrace
        SET(XTool_LINK_LIBS ${XTool_LINK_LIBS} dl)
    ENDIF()
ENDIF()

ADD_LIBRARY(XTool STATIC ${XTool_SRCS})
target_compile_options(XTool PUBLIC ${PERIMETER_COMPILE_OPTIONS})
TARGET_LINK_LIBRARIES(XTool ${XTool_LINK_LIBS})
