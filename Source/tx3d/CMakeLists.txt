project(tx3d)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

# not used
LIST(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/SphericSimplexInterpolator3D-old.cpp)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net)