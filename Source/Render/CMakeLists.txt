project(Render)

file(GLOB SOURCES
  src/*.cpp
  gemsiii/*.c
  client/*.cpp
  D3D/*.cpp
  D3D/clip/*.cpp
  shader/*.cpp
  saver/*.cpp
  3dx/*.cpp)
file(GLOB HEADERS
  src/*.h
  gemsiii/*.h
  inc/*.h
  client/*.h
  D3D/*.h
  D3D/clip/*.cpp
  shader/*.h
  saver/*.h
  3dx/*.h)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR};
  ${CMAKE_CURRENT_SOURCE_DIR}/D3D;
  ${CMAKE_CURRENT_SOURCE_DIR}/Inc;
  ${CMAKE_CURRENT_SOURCE_DIR}/client;
  ${CMAKE_CURRENT_SOURCE_DIR}/Src;
  ${CMAKE_CURRENT_SOURCE_DIR}/../PluginMAX;
  ${CMAKE_CURRENT_SOURCE_DIR}/../Util;
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net;
  ${CMAKE_CURRENT_SOURCE_DIR}/../../MSDXSDK_02_06/Include)
