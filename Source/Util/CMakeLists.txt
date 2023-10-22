add_library(Util STATIC
        BinaryArchive.cpp
        DebugPrm.cpp
        DebugUtil.cpp
        EditArchive.cpp
        MissionDescription.cpp
        SaveSQSH.cpp
        SaveConditions.cpp
        SaveActions.cpp
        SaveOthers.cpp
        Save.cpp
        AVWrapper.cpp
        SynchroTimer.cpp
        SystemUtil.cpp
        XPrmArchive.cpp
        Localization.cpp
        ANIFile.cpp
)

target_include_directories(Util PRIVATE
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/Source/Render/inc"
        "${PROJECT_SOURCE_DIR}/Source/Terra"
        "${PROJECT_SOURCE_DIR}/Source/tx3d"
        "${PROJECT_SOURCE_DIR}/Source/Network"
        "${PROJECT_SOURCE_DIR}/Source/UserInterface"
        "${PROJECT_SOURCE_DIR}/Source/Units"
        "${PROJECT_SOURCE_DIR}/Source/Physics"
        "${PROJECT_SOURCE_DIR}/Source/AI"
        "${PROJECT_SOURCE_DIR}/Source/Sound"
        "${PROJECT_SOURCE_DIR}/Source/Game"
        .
)

target_compile_options(Util PRIVATE ${PERIMETER_COMPILE_OPTIONS})
