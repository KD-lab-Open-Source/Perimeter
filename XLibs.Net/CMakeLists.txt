INCLUDE_DIRECTORIES(
        BEFORE .
)

SET(XLibs_SRCS
        xutil.cpp
        xmath.cpp
        xbuffer.cpp
        xstream.cpp
        plaympp.cpp
        playogg.cpp
)

ADD_LIBRARY(XLibs STATIC ${XLibs_SRCS})
