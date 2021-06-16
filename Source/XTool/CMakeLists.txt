INCLUDE_DIRECTORIES(
        BEFORE .
        ${Boost_INCLUDE_DIRS}
)

SET(XTool_SRCS
        #xtcore.cpp
        xutilcore.cpp
        xmath.cpp
        XBUFFER/XBCNVIN.CPP
        XBUFFER/XBCNVOUT.CPP
        XBUFFER/XBCORE.CPP
        XBUFFER/XBSEARCH.CPP
        XSTREAM/XSCNVOUT.CPP
        XSTREAM/XSINPUT.CPP
        XSTREAM/XSSERV.CPP
        #XSTREAM/XSTIME.CPP
        XSTREAM/XSRDWR.CPP
        XSTREAM/XSCNVIN.CPP
        XSTREAM/XSENLV.CPP
        XSTREAM/XSCORE.CPP
        XSTREAM/XSOUTPUT.CPP
        #XSTREAM/XSGETLIN.CPP
        #XERRHAND/DiagAssert.cpp
        #XERRHAND/BSUFunctions.cpp
        #XERRHAND/IsNT.cpp
        #XERRHAND/CrashHandler.cpp
        #XERRHAND/TLHELPProcessInfo.cpp
        #XERRHAND/NT4ProcessInfo.cpp
        #XERRHAND/GetLoadedModules.cpp
        XERRHAND/xerrhand.CPP
        XUTIL/XUTIL.CPP
        XUTIL/XClock.cpp
)

IF(MSVC_CL_BUILD)
	SET(XTool_SRCS ${XTool_SRCS}
		XERRHAND/DiagAssert.cpp
	)
ENDIF()

ADD_LIBRARY(XTool STATIC ${XTool_SRCS})
