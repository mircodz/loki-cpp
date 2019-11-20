set(BINARY main)
set(CMAKE_CXX_STANDARD 17)

cmake_minimum_required(VERSION 3.12.4)
project(${BINARY})

add_subdirectory(tests)
add_subdirectory(benchmarks)

# set up main executable
file(GLOB SOURCES
  ${PROJECT_SOURCE_DIR}/main.cpp
  ${PROJECT_SOURCE_DIR}/src/*.cpp)

SET(HEADERS
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/external)

# example binary build
add_executable(${BINARY} ${SOURCES})
include_directories(${BINARY} INTERFACE ${HEADERS})

find_package(CURL REQUIRED)
if(CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries(${BINARY} ${CURL_LIBRARIES})
else(CURL_FOUND)
  message(FATAL_ERROR "Could not find libcurl")
endif(CURL_FOUND)

target_link_libraries(${BINARY} fmt)
target_link_libraries(${BINARY} pthread)

#install(TARGETS ${BINARY} DESTINATION bin)

# shared library build
add_library(loki-cpp ${SOURCES})
include_directories(loki-cpp INTERFACE ${HEADERS})

find_package(CURL REQUIRED)
if(CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries(loki-cpp ${CURL_LIBRARIES})
else(CURL_FOUND)
  message(FATAL_ERROR "Could not find libcurl")
endif(CURL_FOUND)

target_link_libraries(loki-cpp fmt)
target_link_libraries(loki-cpp pthread)
#install(TARGETS loki-cpp
    #LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    #PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
