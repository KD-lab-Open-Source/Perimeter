project(Game)

file(GLOB SOURCES
  *.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../UserInterface/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../UserInterface/SCodeInterpreter/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../Util/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../AI/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../Units/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../HT/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../Network/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../Physics/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../GeoLogic/*.cpp
  
  ${CMAKE_CURRENT_SOURCE_DIR}/../TriggerEditor/TriggerExport.cpp)
file(GLOB HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/../UserInterface/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../UserInterface/SCodeInterpreter/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../Util/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../AI/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../Units/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../Network/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../Physics/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../GeoLogic/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../HT/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../TriggerEditor/TriggerExport.h
  *.h)


# not used
LIST(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ConfigPro.cpp)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR};
  ${CMAKE_CURRENT_SOURCE_DIR}/../Util;
  ${CMAKE_CURRENT_SOURCE_DIR}/../Render/inc;
  ${CMAKE_CURRENT_SOURCE_DIR}/../Render/client;
  ${CMAKE_CURRENT_SOURCE_DIR}/../Network;
  ${CMAKE_CURRENT_SOURCE_DIR}/../Units;
  ${CMAKE_CURRENT_SOURCE_DIR}/../UserInterface;
  ${CMAKE_CURRENT_SOURCE_DIR}/../Terra;
  ${CMAKE_CURRENT_SOURCE_DIR}/../Physics;
  ${CMAKE_CURRENT_SOURCE_DIR}/../Sound;
  ${CMAKE_CURRENT_SOURCE_DIR}/../AI;
  ${CMAKE_CURRENT_SOURCE_DIR}/../tx3d;
  ${CMAKE_CURRENT_SOURCE_DIR}/../GeoLogic;
  ${CMAKE_CURRENT_SOURCE_DIR}/Scripts;
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net;
  ${CMAKE_CURRENT_SOURCE_DIR}/../../MSDXSDK_02_06/Include)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net/boost)

target_compile_definitions(${PROJECT_NAME} PRIVATE _PERIMETER_)

target_link_libraries(${PROJECT_NAME} PRIVATE
  PluginMAX
  Terra
  tx3d
  Port
  TriggerEditor
  Peer
  Render
  PerimeterSound)

if(WIN32)
  set(DXLIBS ${CMAKE_CURRENT_SOURCE_DIR}/../../MSDXSDK_02_06/Lib/x86)
  find_library(D3DX name d3dx9 PATHS ${DXLIBS})
  find_library(D3D name d3d9 PATHS ${DXLIBS})
  find_library(DSOUND name dsound PATHS ${DXLIBS})
  find_library(DXGUID name dxguid PATHS ${DXLIBS})
  find_library(DXERR name dxerr PATHS ${DXLIBS})

  target_link_libraries(${PROJECT_NAME} PRIVATE
  winmm
  ws2_32
  legacy_stdio_definitions
  ${D3DX}
  ${D3D}
  ${DSOUND}
  ${DXGUID}
  ${DXERR})
endif()