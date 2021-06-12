INCLUDE_DIRECTORIES(
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/tx3d"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

SET(Terra_SRCS
        crc.cpp
        f3d.cpp
        fastmath.cpp
        geo.cpp
        geoNet.cpp
        grid.cpp
        pn.cpp
        pnint.cpp
        pnt.cpp
        stdafxTr.cpp
        sur_scr.cpp
        tgai.cpp
        tmask.cpp
        tools.cpp
        undoDispatcher.cpp
        worldGen.cpp
        VMAP.CPP
        LAND.CPP
)

ADD_DEFINITIONS(-D_TX3D_LIBRARY_)

ADD_LIBRARY(Terra STATIC ${Terra_SRCS})
