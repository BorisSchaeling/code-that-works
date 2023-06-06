add_executable(steps steps.cpp)
target_link_libraries(steps PRIVATE src gunit gherkin)
add_custom_target(run_tests ALL SCENARIO=${CMAKE_CURRENT_SOURCE_DIR}/validation.feature ${CMAKE_CURRENT_BINARY_DIR}/steps DEPENDS steps)
