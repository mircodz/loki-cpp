find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

file(GLOB SOURCES
  ${PROJECT_SOURCE_DIR}/src/*.cpp
  ${PROJECT_SOURCE_DIR}/tests/*.cpp)

set(HEADERS
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/external)

add_executable(tests ${SOURCES})
include_directories(tests INTERFACE ${HEADERS})

find_package(CURL REQUIRED)
if(CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries(tests ${CURL_LIBRARIES})
else(CURL_FOUND)
  message(FATAL_ERROR "Could not find libcurl")
endif(CURL_FOUND)

target_link_libraries(tests fmt)
target_link_libraries(tests pthread)

add_test(tests tests)

add_custom_target(check
  COMMAND ${CMAKE_CTEST_COMMAND}
  DEPENDS tests)
