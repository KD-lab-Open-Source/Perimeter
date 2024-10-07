
IF (MACOS) #TODO Fix this for mac
    OPTION(OPTION_USE_BACKTRACE "Use libbacktrace if present" OFF)
ELSE()
    OPTION(OPTION_USE_BACKTRACE "Use libbacktrace if present" ON)
ENDIF ()

#GameMath retrieval
FetchContent_Declare(gamemath
		GIT_REPOSITORY    https://github.com/caiiiycuk/perimeter-gamemath
		GIT_TAG           "155ab00471b10b0a1e19c18588e6ccf9a356cb8a"
		GIT_SHALLOW       OFF
)
FetchContent_MakeAvailable(gamemath)

SET(XTool_LINK_LIBS gamemath ZLIB::ZLIB)

IF(OPTION_DISABLE_STACKTRACE)
    MESSAGE("Stacktrace: Disabled")
ELSE()
    find_package(Boost 1.76.0)
    IF(NOT Boost_FOUND)
        message(SEND_ERROR "Stacktrace: No Boost found and OPTION_DISABLE_STACKTRACE is not set")
    ELSE()
        #https://www.boost.org/doc/libs/1_78_0/doc/html/stacktrace/configuration_and_build.html
        INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})
        
        macro(setup_backtrace)
            MESSAGE("Stacktrace: Using Backtrace")
            ADD_DEFINITIONS(-DBOOST_STACKTRACE_USE_BACKTRACE)
            if(Backtrace_INCLUDE_DIRS)
                include_directories(${Backtrace_INCLUDE_DIRS})
            endif()
            if(Backtrace_INCLUDE_DIR)
                include_directories(${Backtrace_INCLUDE_DIR})
            endif()
            if(NOT Backtrace_LIBRARIES)
                message("Backtrace_LIBRARIES not filled, using builtin")
                set(Backtrace_LIBRARIES -lbacktrace)
            endif()
            SET(XTool_LINK_LIBS ${XTool_LINK_LIBS} ${Backtrace_LIBRARIES})
        endmacro()
        macro(setup_windbg)
            MESSAGE("Stacktrace: Using WinDbg")
            ADD_DEFINITIONS(-DBOOST_STACKTRACE_USE_WINDBG)
            SET(XTool_LINK_LIBS ${XTool_LINK_LIBS} ole32 dbgeng)
        endmacro()
        
        IF(MSVC_CL_BUILD)
            #Use WinDbg for MSVC
            setup_windbg()
        ELSEIF(PERIMETER_WINDOWS)
            #Use Backtrace if available in Msys/Mingw, else WinDbg
            SET(CMAKE_FIND_LIBRARY_SUFFIXES ${PREFER_STATIC_LIBRARY_SUFFIXES})
            FIND_PACKAGE(Backtrace)
            IF(Backtrace_FOUND AND OPTION_USE_BACKTRACE)
                setup_backtrace()
            ELSE()
                setup_windbg()
            ENDIF()
            SET(CMAKE_FIND_LIBRARY_SUFFIXES ${PREFER_DYNAMIC_LIBRARY_SUFFIXES})
        ELSE()
            #Use ADDR2LINE in POSIX unless Backtrace is wanted
            FIND_PACKAGE(Backtrace)
            IF(Backtrace_FOUND AND OPTION_USE_BACKTRACE)
                setup_backtrace()
            ELSE()
                MESSAGE("Stacktrace: Using Addr2Line")
                ADD_DEFINITIONS(-DBOOST_STACKTRACE_USE_ADDR2LINE)
                # dl is required for dladdr in stacktrace
                SET(XTool_LINK_LIBS ${XTool_LINK_LIBS} dl)
            ENDIF()
        ENDIF()
    ENDIF()
ENDIF()

add_library(XTool STATIC
        xmath/xmath.cpp
        xmath/gamemath.cpp
        xmath/std.cpp
        XBUFFER/XBCNVIN.cpp
        XBUFFER/XBCNVOUT.cpp
        XBUFFER/XBCORE.cpp
        XBUFFER/XBSEARCH.cpp
        XSTREAM/XSCNVOUT.cpp
        XSTREAM/XSSERV.cpp
        XSTREAM/XSRDWR.cpp
        XSTREAM/XSCNVIN.cpp
        XSTREAM/XSENLV.cpp
        XSTREAM/XSCORE.cpp
        xerrhand.cpp
        XUTIL/XUTIL.cpp
        XUTIL/XClock.cpp
        files/files.cpp
        codepages/codepages.cpp
)

target_compile_options(XTool PRIVATE ${PERIMETER_COMPILE_OPTIONS})

target_link_libraries(XTool PRIVATE ${XTool_LINK_LIBS})

target_include_directories(XTool PRIVATE
        .
        "${gamemath_SOURCE_DIR}/include"
)

IF(OPTION_DISABLE_STACKTRACE)
    target_compile_definitions(XTool PUBLIC -DOPTION_DISABLE_STACKTRACE)
ENDIF()
