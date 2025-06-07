# Test Example
add_executable(test_example
    test_example.cpp
    #../src/your_file.cpp
)

target_include_directories(test_example PRIVATE
    ../include
)

target_link_libraries(test_example
    gtest
    gtest_main
)

add_test(NAME TestExample COMMAND test_example)
