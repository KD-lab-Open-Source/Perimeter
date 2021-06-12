INCLUDE_DIRECTORIES(
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

SET(Sound_SRCS
        C3D.cpp
        init.cpp
        SoftwareBuffer.cpp
        SoundScript.cpp
        StdAfx.cpp
        WaveFile.cpp
)

ADD_LIBRARY(Sound STATIC ${Sound_SRCS})
