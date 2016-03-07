enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test test.cpp)
target_link_libraries(test LINK_PUBLIC cow_instrument ${GTEST_BOTH_LIBRARIES})
add_test(test test)
