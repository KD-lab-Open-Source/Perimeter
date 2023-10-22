add_library(Sound STATIC
        SoundScript.cpp
        C3D.cpp
        PerimeterSound.cpp
        SoftwareBuffer.cpp
        SoundScript.cpp
        Sample.cpp
        AudioPlayer.cpp
)

target_include_directories(Sound PRIVATE
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

target_compile_options(Sound PRIVATE ${PERIMETER_COMPILE_OPTIONS})
