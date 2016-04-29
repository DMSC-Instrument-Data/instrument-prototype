enable_testing()

# Auto-find correct threading library
find_package(Threads)

# Location of googletest (by default, expected adjacent to instrument-prototype)
get_filename_component( DEFAULT_GTEST_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../googletest" REALPATH )
get_filename_component( DEFAULT_GTEST_BUILD_DIR "${CMAKE_SOURCE_DIR}/../googletest" REALPATH )
set( GTEST_SOURCE_DIR "${DEFAULT_GTEST_SOURCE_DIR}" CACHE PATH "GoogleTest top directory" )
set( GTEST_BUILD_DIR "${DEFAULT_GTEST_BUILD_DIR}" CACHE PATH "GoogleTest build directory" )

# GTest config
get_filename_component( GTEST_INCLUDE_DIR "${GTEST_SOURCE_DIR}/googletest/include" REALPATH )
get_filename_component( GTEST_LIBRARY "${GTEST_BUILD_DIR}/googlemock/gtest/libgtest.a" REALPATH )
get_filename_component( GTEST_MAIN_LIBRARY "${GTEST_BUILD_DIR}/googlemock/gtest/libgtest_main.a" REALPATH )

# GMock config
get_filename_component( GMOCK_INCLUDE_DIR "${GTEST_SOURCE_DIR}/googlemock/include" REALPATH )
get_filename_component( GMOCK_LIBRARY "${GTEST_BUILD_DIR}/googlemock/libgmock.a" REALPATH )
get_filename_component( GMOCK_MAIN_LIBRARY "${GTEST_BUILD_DIR}/googlemock/libgmock_main.a" REALPATH )

# Validate directories
if( NOT EXISTS "${GTEST_INCLUDE_DIR}/" OR NOT EXISTS "${GMOCK_INCLUDE_DIR}/" )
    message( SEND_ERROR "GoogleTest include directories missing. \nEnsure GTEST_SOURCE_DIR is correctly set to GoogleTest top directory." )
endif()

if( NOT EXISTS "${GTEST_LIBRARY}" OR NOT EXISTS "${GTEST_MAIN_LIBRARY}" OR NOT EXISTS "${GMOCK_LIBRARY}" OR NOT EXISTS "${GMOCK_MAIN_LIBRARY}" )
    message( SEND_ERROR "GoogleTest libraries not found. \nEnsure GTEST_BUILD_DIR is correctly set to GoogleTest build directory." )
endif()

# Set include directories
include_directories(${GTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR})

set ( TEST_FILES
                 CompositeComponentTest.cpp
                 DetectorComponentTest.cpp
                 InstrumentTreeTest.cpp
                 #MoveCommandTest.cpp
                 #NodeIteratorTest.cpp
                 NodeTest.cpp
)

add_executable(cow_instrument_test runner.cpp MockTypes.h ${TEST_FILES})

target_link_libraries(cow_instrument_test LINK_PUBLIC cow_instrument ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${GMOCK_MAIN_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

add_test(cow_instrument_test cow_instrument_test)

