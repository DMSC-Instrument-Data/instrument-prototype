enable_testing()
find_package(GTest REQUIRED)


get_filename_component(GTEST_LIBRARY_DIR ${GTEST_LIBRARY} DIRECTORY)
MESSAGE(STATUS ${GTEST_LIBRARY_DIR})
set( GMOCK_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}/../../googlemock/include/ CACHE FILEPATH "gmock include dir")
set( GMOCK_LIBRARIES ${GTEST_LIBRARY_DIR}/../libgmock.a CACHE FILEPATH  "libgmock.a")
set( GMOCK_MAIN_LIBRARIES ${GTEST_LIBRARY_DIR}/../libgmock-main.a CACHE FILEPATH  "libgmock-main.a")

include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})


set ( TEST_FILES MoveCommandTest.cpp
                 DetectorTest.cpp
                 NodeTest.cpp
                 Example.cpp
                 CompositeComponentTest.cpp
)

add_executable(cow_instrument_test runner.cpp MockTypes.h ${TEST_FILES})
target_link_libraries(cow_instrument_test LINK_PUBLIC cow_instrument ${GTEST_BOTH_LIBRARIES} ${GMOCK_LIBRARIES} ${GMOCK_MAIN_LIBRARIES})
add_test(cow_instrument_test cow_instrument_test)

