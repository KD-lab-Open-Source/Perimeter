
INCLUDE_DIRECTORIES(
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

SET(Exodus_SRCS
        exodus.cpp
        ${pevents_SOURCE_DIR}/src/pevents.cpp
)

IF(OPTION_D3D9)
    SET(Exodus_SRCS ${Exodus_SRCS} d3dx9.cpp)
ENDIF()

ADD_LIBRARY(Exodus STATIC ${Exodus_SRCS})
target_compile_options(Exodus PRIVATE ${PERIMETER_COMPILE_OPTIONS})

