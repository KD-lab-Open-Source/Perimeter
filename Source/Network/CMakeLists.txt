project(Peer)

file(GLOB SOURCES
  Peer2/*.c
  Peer2/peer/*.c
  Peer2/pinger/*.c
  Peer2/chat/*.c
  Peer2/qr2/*.c
  Peer2/serverbrowsing/*.c)
file(GLOB HEADERS
  Peer2/*.h
  Peer2/peer/*.h
  Peer2/pinger/*.h
  Peer2/chat/*.h
  Peer2/qr2/*.h
  Peer2/serverbrowsing/*.h)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../XLibs.net;)
