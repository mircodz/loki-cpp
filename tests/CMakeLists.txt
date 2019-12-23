include(CTest)
include(Catch)

file(GLOB SOURCES
  ${PROJECT_SOURCE_DIR}/src/*.cpp
  ${PROJECT_SOURCE_DIR}/tests/*.cpp)

file(GLOB_RECURSE PROTOBUF_GEN
  ${PROJECT_SOURCE_DIR}/gen/*.pb.cc)

set(HEADERS
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/gen)

add_executable(tests ${SOURCES} ${PROTOBUF_GEN})
include_directories(tests INTERFACE ${HEADERS})

target_link_libraries(tests loki-cpp)

add_test(tests tests)

add_custom_target(check
  COMMAND ${CMAKE_CTEST_COMMAND}
  DEPENDS tests)
