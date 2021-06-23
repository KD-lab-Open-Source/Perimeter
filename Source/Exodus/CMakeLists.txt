INCLUDE_DIRECTORIES(
    BEFORE .
    "${PROJECT_SOURCE_DIR}/Source/XTool"
)

SET(Exodus_SRCS
        exodus.cpp
        d3dx9.cpp
        d3dx9math.cpp
)

ADD_LIBRARY(Exodus STATIC ${Exodus_SRCS})
