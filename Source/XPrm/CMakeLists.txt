# XPrmLib executable

add_library(XPrmLib STATIC
        StdAfx.cpp
        Token.cpp
        RunTimeCompiler.cpp
)

target_include_directories(XPrmLib PRIVATE
        .
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/Source/Util"
)

target_compile_options(XPrmLib PRIVATE ${PERIMETER_COMPILE_OPTIONS})

# XPrm executable

add_executable(XPrm ${XPrm_SRCS} XPrm.cpp)

target_include_directories(XPrm PRIVATE
        .
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/Source/Util"
)


set(XPrm_LINK_LIBS ${EXE_LINK_LIBS_PRE} XPrmLib XTool ${SDL2_LIBRARY} ${EXE_LINK_LIBS_POST})

IF(PERIMETER_EXODUS)
    SET(XPrm_LINK_LIBS ${XPrm_LINK_LIBS} Exodus)
ENDIF()

target_link_libraries(XPrm PRIVATE ${XPrm_LINK_LIBS})

target_compile_options(XPrm PRIVATE ${PERIMETER_COMPILE_OPTIONS})

