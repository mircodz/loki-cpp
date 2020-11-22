find_package(GTest)

if(GTEST_FOUND)
  include(GoogleTest)

  file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/src/registry.cpp
    ${PROJECT_SOURCE_DIR}/src/agent.cpp
    ${PROJECT_SOURCE_DIR}/src/query.cpp
    ${PROJECT_SOURCE_DIR}/src/detail/utils.cpp
    ${PROJECT_SOURCE_DIR}/gen/github.com/gogo/protobuf/gogoproto/gogo.pb.cc
    ${PROJECT_SOURCE_DIR}/gen/google/protobuf/descriptor.pb.cc
    ${PROJECT_SOURCE_DIR}/gen/google/protobuf/timestamp.pb.cc
    ${PROJECT_SOURCE_DIR}/gen/logproto.pb.cc

    ${PROJECT_SOURCE_DIR}/tests/main.cpp)

  set(HEADERS
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/gen)

  add_executable(unittest ${SOURCES})
  target_include_directories(unittest PRIVATE ${HEADERS})
  target_link_libraries(unittest gtest)
  target_link_libraries(unittest fmt)

  find_package(Protobuf REQUIRED)
  if(PROTOBUF_FOUND)
    target_link_libraries(unittest snappy)
    target_link_libraries(unittest ${PROTOBUF_LIBRARIES})
  else()
    message("Could not find protobof")
  endif()

  find_package(CURL REQUIRED)
  if(CURL_FOUND)
    target_link_libraries(unittest ${CURL_LIBRARIES})
  else()
    message(FATAL_ERROR "Could not find libcurl")
  endif()

  gtest_add_tests(TARGET unittest)
endif()
