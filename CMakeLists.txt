cmake_minimum_required(VERSION 3.12.4)
project(loki-cpp VERSION 0.2.1)

include(GNUInstallDirs)

# CMake Settings
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

# Project Settings
option(BUILD_SHARED_LIBS "Build libraries as shared" ON)
option(BUILD_TESTS       "Build unit testing"        OFF)

# Compiler Settings
set(CMAKE_CXX_STANDARD   17)
set(CMAKE_CXX_EXTENSIONS Off)

if(BUILD_TESTS)
	add_subdirectory(tests)
endif()

file(GLOB SOURCES
	${PROJECT_SOURCE_DIR}/src/registry.cpp
	${PROJECT_SOURCE_DIR}/src/agent.cpp
	${PROJECT_SOURCE_DIR}/src/detail/utils.cpp
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

set_target_properties(loki-cpp PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION 1
	PUBLIC_HEADER "include/parser.hpp;include/builder.hpp;include/registry.hpp;include/detail/utils.hpp;include/agent.hpp")

find_package(Protobuf REQUIRED)
if(PROTOBUF_FOUND)
	target_link_libraries(loki-cpp snappy)
	target_link_libraries(loki-cpp ${PROTOBUF_LIBRARIES})
	include_directories(${PROTOBUF_INCLUDE_DIRS})
	function(compile_proto _file)
		execute_process(
			COMMAND protoc --cpp_out=${PROJECT_SOURCE_DIR}/gen
										--proto_path=${PROJECT_SOURCE_DIR}/proto
										${_file}
			RESULT_VARIABLE result)
		if(${result})
			message(FATAL_ERROR "Error compiling protobuf schemas.")
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

target_link_libraries(loki-cpp fmt)
target_link_libraries(loki-cpp pthread)

install(TARGETS loki-cpp
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION /usr/local/include/loki OPTIONAL)

configure_file(loki-cpp.pc.in loki-cpp.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/loki-cpp.pc
	DESTINATION /usr/lib/pkgconfig)
