INCLUDE_DIRECTORIES(
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

SET(Sound_SRCS
        SoundScript.cpp
        C3D.cpp
        PerimeterSound.cpp
        SoftwareBuffer.cpp
        SoundScript.cpp
        Sample.cpp
        AudioPlayer.cpp
)

ADD_LIBRARY(Sound STATIC ${Sound_SRCS})
target_compile_options(Sound PRIVATE ${PERIMETER_COMPILE_OPTIONS})
