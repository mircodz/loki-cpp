file(GLOB SOURCES
  ${PROJECT_SOURCE_DIR}/src/*.cpp
  ${PROJECT_SOURCE_DIR}/benchmarks/*.cpp)

set(HEADERS
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/external)

add_executable(benchmarks ${SOURCES})
include_directories(benchmarks INTERFACE ${HEADERS})

find_package(CURL REQUIRED)
if(CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries(benchmarks ${CURL_LIBRARIES})
else(CURL_FOUND)
  message(FATAL_ERROR "Could not find libcurl")
endif(CURL_FOUND)

target_link_libraries(benchmarks benchmark)
target_link_libraries(benchmarks pthread)
target_link_libraries(benchmarks fmt)

add_custom_target(bench
  COMMAND benchmarks
  DEPENDS benchmarks)
