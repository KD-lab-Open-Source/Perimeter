project(PluginMAX)

file(GLOB SOURCES *.cpp Src/*.cpp)
file(GLOB HEADERS *.h Inc/*.h)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR};
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net)
