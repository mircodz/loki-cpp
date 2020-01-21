find_package(Catch2 REQUIRED)
if(TARGET Catch2::Catch2)

  include(CTest)
  include(Catch)

  file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/tests/main.cpp
    ${PROJECT_SOURCE_DIR}/tests/parser.cpp)

  add_executable(tests ${SOURCES})
  include_directories(tests PRIVATE ${HEADERS})

  target_link_libraries(tests loki-cpp)
  add_test(tests tests)

  add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS tests)

else()
endif()
