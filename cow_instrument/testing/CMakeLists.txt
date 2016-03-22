enable_testing()

# Auto-find correct threading library
find_package(Threads)

# TOP Directory of googletest (currently expected adjacent to instrument-prototype)
get_filename_component( GTEST_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../googletest REALPATH CACHE )
get_filename_component( GTEST_BUILD_DIR ${CMAKE_SOURCE_DIR}/../googletest REALPATH CACHE )

# GTest config
get_filename_component( GTEST_INCLUDE_DIR ${GTEST_SOURCE_DIR}/googletest/include REALPATH CACHE )
get_filename_component( GTEST_LIBRARY ${GTEST_BUILD_DIR}/googlemock/gtest/libgtest.a REALPATH CACHE )
get_filename_component( GTEST_MAIN_LIBRARY ${GTEST_BUILD_DIR}/googlemock/gtest/libgtest_main.a REALPATH CACHE )

# GMock config
get_filename_component( GMOCK_INCLUDE_DIR ${GTEST_SOURCE_DIR}/googlemock/include REALPATH CACHE )
get_filename_component( GMOCK_LIBRARY ${GTEST_BUILD_DIR}/googlemock/libgmock.a REALPATH CACHE )
get_filename_component( GMOCK_MAIN_LIBRARY ${GTEST_BUILD_DIR}/googlemock/libgmock_main.a REALPATH CACHE )

# Set include directories
include_directories(${GTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR})

set ( TEST_FILES
                 CompositeComponentTest.cpp
                 DetectorComponentTest.cpp
                 Example.cpp
                 InstrumentTreeTest.cpp
                 MoveCommandTest.cpp
                 NodeIteratorTest.cpp
                 NodeTest.cpp
)

add_executable(cow_instrument_test runner.cpp MockTypes.h ${TEST_FILES})

target_link_libraries(cow_instrument_test LINK_PUBLIC cow_instrument ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${GMOCK_MAIN_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

add_test(cow_instrument_test cow_instrument_test)

