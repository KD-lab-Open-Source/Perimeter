INCLUDE_DIRECTORIES(
    BEFORE .
)

SET(Exodus_SRCS
        exodus.cpp
        sound.cpp
)

ADD_LIBRARY(Exodus STATIC ${Exodus_SRCS})
