cmake_minimum_required(VERSION 3.12.4)

project(loki-cpp VERSION 0.1.0)

option(BUILD_SHARED_LIBS "Build libraries as shared" ON)

set(CMAKE_CXX_STANDARD   17)
set(CMAKE_CXX_EXTENSIONS Off)

find_package(benchmark REQUIRED)
if(benchmark_FOUND)
  add_subdirectory(benchmarks)
else()
endif()

find_package(Catch2 REQUIRED)
if(TARGET Catch2::Catch2)
  add_subdirectory(tests)
else()
endif()

set(PROTOBUF_PROTO
  logproto.proto
  google/protobuf/timestamp.proto
  google/protobuf/descriptor.proto
  github.com/gogo/protobuf/gogoproto/gogo.proto)

foreach(proto ${PROTOBUF_PROTO})
  execute_process(
    COMMAND protoc --cpp_out=${PROJECT_SOURCE_DIR}/gen
                   --proto_path=${PROJECT_SOURCE_DIR}/proto
                   ${proto}
    RESULT_VARIABLE result)
  if(${result})
    message(FATAL_ERROR "Error compiling protobuf schemas.")
  endif()
endforeach()

file(GLOB SOURCES
  ${PROJECT_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE PROTOBUF_GEN
  ${PROJECT_SOURCE_DIR}/gen/*.pb.cc)

set(HEADERS
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/gen)

add_library(loki-cpp ${SOURCES} ${PROTOBUF_GEN})
target_include_directories(loki-cpp PRIVATE ${HEADERS})
target_compile_options(loki-cpp PRIVATE -Wall -Wextra)

set_target_properties(loki-cpp PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION 1
  PUBLIC_HEADER "include/agent.hpp;include/common.hpp;include/parser.hpp;include/builder.hpp;include/registry.hpp")

find_package(Protobuf REQUIRED)
if(PROTOBUF_FOUND)
  include_directories(${PROTOBUF_INCLUDE_DIRS})
  target_link_libraries(loki-cpp ${PROTOBUF_LIBRARIES})
else()
  message(FATAL_ERROR "Could not find protobof")
endif()

find_package(CURL REQUIRED)
if(CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries(loki-cpp ${CURL_LIBRARIES})
else()
  message(FATAL_ERROR "Could not find libcurl")
endif()

target_link_libraries(loki-cpp fmt)
target_link_libraries(loki-cpp pthread)
target_link_libraries(loki-cpp snappy)

install(TARGETS loki-cpp
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION /usr/local/include/loki)

# pkg-config
configure_file(loki-cpp.pc.in loki-cpp.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/loki-cpp.pc DESTINATION /usr/lib/pkgconfig)
