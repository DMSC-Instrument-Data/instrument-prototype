enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})


set ( TEST_FILES InstrumentTest.cpp
                 DetectorTest.cpp
)

add_executable(cow_instrument_test ${TEST_FILES})
target_link_libraries(cow_instrument_test LINK_PUBLIC cow_instrument ${GTEST_BOTH_LIBRARIES})
add_test(cow_instrument_test cow_instrument_test)
