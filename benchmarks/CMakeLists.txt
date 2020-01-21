find_package(benchmark REQUIRED)
if(benchmark_FOUND)

  file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/benchmarks/main.cpp
    ${PROJECT_SOURCE_DIR}/benchmarks/parser.cpp)

  add_executable(bench ${SOURCES})
  include_directories(bench INTERFACE ${HEADERS})

  target_link_libraries(bench benchmark)
  target_link_libraries(bench pthread)
  target_link_libraries(bench loki-cpp)

else()
endif()
