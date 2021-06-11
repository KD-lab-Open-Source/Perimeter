project(PerimeterSound)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})


target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../Util;
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net)


target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net/boost)
