project(Terra)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

#find_package(Boost REQUIRED)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net)

#target_link_libraries(${PROJECT_NAME} PRIVATE Boost::headers)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net/boost)

target_compile_definitions(${PROJECT_NAME} PRIVATE _PERIMETER_)