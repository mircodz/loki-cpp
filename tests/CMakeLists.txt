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

#find_package(Protobuf REQUIRED)
#if(PROTOBUF_FOUND)
  #include_directories(${PROTOBUF_INCLUDE_DIRS})
  #target_link_libraries(tests ${PROTOBUF_LIBRARIES})
#else()
  #message(FATAL_ERROR "Could not find protobof")
#endif()

#find_package(CURL REQUIRED)
#if(CURL_FOUND)
  #include_directories(${CURL_INCLUDE_DIR})
  #target_link_libraries(tests ${CURL_LIBRARIES})
#else()
  #message(FATAL_ERROR "Could not find libcurl")
#endif()

target_link_libraries(tests loki-cpp)
#target_link_libraries(tests fmt)
#target_link_libraries(tests snappy)

add_test(tests tests)

add_custom_target(check
  COMMAND ${CMAKE_CTEST_COMMAND}
  DEPENDS tests)
