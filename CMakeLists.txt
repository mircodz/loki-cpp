cmake_minimum_required (VERSION 2.8.11)
project (loki-cpp)

set(CMAKE_CXX_STANDARD 17)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

file(GLOB SOURCES
  src/*.cpp
  main.cpp
)

add_executable(loki-cpp ${SOURCES})

target_link_libraries(loki-cpp fmt)
