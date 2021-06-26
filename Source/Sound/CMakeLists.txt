INCLUDE_DIRECTORIES(
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

SET(Sound_SRCS
        SoundScript.cpp
)

IF(NOT PERIMETER_EXODUS_SOUND)
SET(Sound_SRCS ${Sound_SRCS}
        C3D.cpp
        init.cpp
        SoftwareBuffer.cpp
        SoundScript.cpp
        StdAfx.cpp
        WaveFile.cpp
)
ENDIF()

ADD_LIBRARY(Sound STATIC ${Sound_SRCS})
