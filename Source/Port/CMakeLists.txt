project(Port)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../Game;
  ${CMAKE_CURRENT_SOURCE_DIR}/../Util;
  ${CMAKE_CURRENT_SOURCE_DIR}/../Render/inc;
  ${CMAKE_CURRENT_SOURCE_DIR}/../Network;
  ${CMAKE_CURRENT_SOURCE_DIR}/../Units;
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net;
  ${CMAKE_CURRENT_SOURCE_DIR}/../../MSDXSDK_02_06/Include)