file(GLOB SOURCES
  ${PROJECT_SOURCE_DIR}/src/*
  ${PROJECT_SOURCE_DIR}/src/detail/*
  ${PROJECT_SOURCE_DIR}/benchmarks/*)

file(GLOB_RECURSE PROTOBUF_GEN
  ${PROJECT_SOURCE_DIR}/gen/*.pb.cc)

set(HEADERS
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/gen)

add_executable(benchmarks ${SOURCES} ${PROTOBUF_GEN})
include_directories(benchmarks INTERFACE ${HEADERS})

target_link_libraries(benchmarks benchmark)
target_link_libraries(benchmarks pthread)

target_link_libraries(benchmarks loki-cpp)

add_custom_target(bench
  COMMAND benchmarks
  DEPENDS benchmarks)
