enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})


set ( TEST_FILES InstrumentTest.cpp)

add_executable(test ${TEST_FILES})
target_link_libraries(test LINK_PUBLIC cow_instrument ${GTEST_BOTH_LIBRARIES})
add_test(test test)
