cmake_minimum_required(VERSION 3.12.4)
project(loki-cpp VERSION 0.4.0)

include(GNUInstallDirs)

# CMake Settings
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

# Project Settings
option(BUILD_SHARED_LIBS "Build libraries as shared" On)
option(BUILD_TESTS       "Build unit testing"        Off)

# Compiler Settings
set(CMAKE_CXX_COMPILER             "/usr/bin/g++")
set(CMAKE_CXX_STANDARD             17)
set(CMAKE_CXX_STANDARD_REQUIRED    On)
set(CMAKE_CXX_EXTENSIONS           Off)
set(CMAKE_CXX_FLAGS_DEBUG          "-g")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os")
set(CMAKE_CXX_FLAGS_RELEASE        "-O4")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

# Testing
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

file(GLOB SOURCES
	${PROJECT_SOURCE_DIR}/src/agent.cpp
	${PROJECT_SOURCE_DIR}/src/detail/utils.cpp
	${PROJECT_SOURCE_DIR}/src/query.cpp

	${PROJECT_SOURCE_DIR}/gen/github.com/gogo/protobuf/gogoproto/gogo.pb.cc
	${PROJECT_SOURCE_DIR}/gen/google/protobuf/descriptor.pb.cc
	${PROJECT_SOURCE_DIR}/gen/google/protobuf/timestamp.pb.cc
	${PROJECT_SOURCE_DIR}/gen/logproto.pb.cc)

set(HEADERS
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/gen)

add_library(loki-cpp ${SOURCES})
target_include_directories(loki-cpp PRIVATE ${HEADERS})
target_compile_options(loki-cpp PRIVATE -Wall -Wextra)

find_package(Protobuf)
if(PROTOBUF_FOUND)
  #find_package(snappy REQUIRED)
  #if(SNAPPY_FOUND)
  #  target_link_libraries(loki-cpp ${Snappy_INCLUDE_DIR})
  #  include_directories(${Snappy_LIBRARIES})
  #else()
	#  message(FATAL_ERROR "Could not find snappy")
  #endif()
	target_link_libraries(loki-cpp snappy)

	target_link_libraries(loki-cpp ${PROTOBUF_LIBRARIES})
	include_directories(${PROTOBUF_INCLUDE_DIRS})

	# Compile .proto files
	function(compile_proto _file)
		execute_process(
			COMMAND protoc 
				--cpp_out=${PROJECT_SOURCE_DIR}/gen
				--proto_path=${PROJECT_SOURCE_DIR}/proto
				${_file}
			RESULT_VARIABLE result)
		if(${result})
			message("Error compiling protobuf schemas.")
		endif()
	endfunction()

	set(PROTOBUF_PROTO
		github.com/gogo/protobuf/gogoproto/gogo.proto
		google/protobuf/descriptor.proto
		google/protobuf/timestamp.proto
		logproto.proto)

	foreach(_file ${PROTOBUF_PROTO})
		compile_proto(${_file})
	endforeach()
else()
	message("Could not find protobof")
endif()

find_package(CURL REQUIRED)
if(CURL_FOUND)
	include_directories(${CURL_INCLUDE_DIR})
	target_link_libraries(loki-cpp ${CURL_LIBRARIES})
else()
	message(FATAL_ERROR "Could not find libcurl")
endif()

find_package(fmt REQUIRED)
IF(fmt_FOUND)
  target_link_libraries(loki-cpp fmt::fmt)
else()
  message(FATAL_ERROR "Could not find libfmt")
endif()

set(THREADS_PREFER_PTHREAD_FLAG On)
find_package(Threads REQUIRED)
if(Threads_FOUND)
  target_link_libraries(loki-cpp Threads::Threads)
else()
  message(FATAL_ERROR "Could not find proper threading library")
endif()

# Installation settings
set_target_properties(loki-cpp PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION 1
	PUBLIC_HEADER "include/builder.hpp;include/registry.hpp;include/agent.hpp"
  PRIVATE_HEADER "include/detail/utils.hpp")

install(TARGETS loki-cpp
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION /usr/local/include/loki OPTIONAL
  PRIVATE_HEADER DESTINATION /usr/local/include/loki/detail)

configure_file(loki-cpp.pc.in loki-cpp.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/loki-cpp.pc
	DESTINATION /usr/lib/pkgconfig)
