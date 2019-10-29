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

find_package(CURL REQUIRED)
if(CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries(loki-cpp ${CURL_LIBRARIES})
else(CURL_FOUND)
  message(FATAL_ERROR "Could not find libcurl")
endif(CURL_FOUND)
