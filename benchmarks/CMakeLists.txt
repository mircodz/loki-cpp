file(GLOB SOURCES
  ${PROJECT_SOURCE_DIR}/src/*.cpp
  ${PROJECT_SOURCE_DIR}/benchmarks/*.cpp)

file(GLOB_RECURSE PROTOBUF_GEN
  ${PROJECT_SOURCE_DIR}/gen/*.pb.cc)

set(HEADERS
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/gen)

add_executable(benchmarks ${SOURCES} ${PROTOBUF_GEN})
include_directories(benchmarks INTERFACE ${HEADERS})

#find_package(Protobuf REQUIRED)
#if(PROTOBUF_FOUND)
  #include_directories(${PROTOBUF_INCLUDE_DIRS})
  #target_link_libraries(benchmarks ${PROTOBUF_LIBRARIES})
#else()
  #message(FATAL_ERROR "Could not find protobof")
#endif()

#find_package(CURL REQUIRED)
#if(CURL_FOUND)
  #include_directories(${CURL_INCLUDE_DIR})
  #target_link_libraries(benchmarks ${CURL_LIBRARIES})
#else()
  #message(FATAL_ERROR "Could not find libcurl")
#endif()

target_link_libraries(benchmarks benchmark)
#target_link_libraries(benchmarks fmt)
target_link_libraries(benchmarks pthread)
#target_link_libraries(benchmarks snappy)

target_link_libraries(benchmarks loki-cpp)

add_custom_target(bench
  COMMAND benchmarks
  DEPENDS benchmarks)
