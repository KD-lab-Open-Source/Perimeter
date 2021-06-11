project(TriggerEditor)

file(GLOB SOURCES
  *.cpp
  *.def
  Main/*.cpp
  Scale/*.cpp
  Tree/*.cpp
  Tree/TreeNodes/*.cpp
  "Custom Controls/*.cpp"
  Utils/*.cpp
  TEEngine/*.cpp
  TEEngine/Profiler/*.cpp
  TEEngine/UndoManager/*.cpp
  TEEngine/Actions/*.cpp
  TEEngine/Drawers/*.cpp
  TEEngine/TEUtils/*.cpp
  TEEngine/WorkModes/*.cpp
  TEEngine/WorkModes/WMUtils/*.cpp
  TEEngine/Profiler/TriggerChainProfiler/*.cpp)
file(GLOB HEADERS
  *.h
  Main/*.h
  Scale/*.h
  Tree/*.h
  Tree/TreeNodes/*.h
  "Custom Controls/*.h"
  Utils/*.h
  TEEngine/*.h
  TEEngine/Profiler/*.h
  TEEngine/UndoManager/*.h
  TEEngine/Actions/*.h
  TEEngine/Drawers/*.h
  TEEngine/TEUtils/*.h
  TEEngine/WorkModes/*.h
  TEEngine/WorkModes/WMUtils/*.h
  TEEngine/Profiler/TriggerChainProfiler/*.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})


target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR};
  ${CMAKE_CURRENT_SOURCE_DIR}/../Util;
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net/boost)

target_compile_definitions(${PROJECT_NAME} PRIVATE _TRIGGER_EDITOR)
target_link_libraries(${PROJECT_NAME} PRIVATE Port)

