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
include_directories(${GTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${MPI_CXX_INCLUDE_PATH})

set ( TEST_FILES
                 CompositeComponentTest.cpp
                 ComponentInfoTest.cpp
                 ComponentProxyTest.cpp
                 DetectorComponentTest.cpp
                 DetectorInfoTest.cpp                 
                 EigenTest.cpp
                 FixedLengthVectorTest.cpp                 
                 IndexTranslatorTest.cpp
                 FlatTreeTest.cpp
                 LinkedTreeParserTest.cpp
                 ParabolicGuideTest.cpp
                 PathComponentTest.cpp
                 PathComponentInfoTest.cpp
                 PointPathComponentTest.cpp
                 ScanTimeTest.cpp
                 SourceSampleDetectorPathFactoryTest.cpp                 
                 SpectrumInfoTest.cpp
                 SpectrumTest.cpp
)


set ( TEST_FILES_MAPPERS
                CompositeComponentMapperTest.cpp
                ComponentMapperTest.cpp
                DetectorComponentMapperTest.cpp
                FlatTreeMapperTest.cpp
                IntToTypeMapperTest.cpp
                NullComponentMapperTest.cpp
                ParabolicGuideMapperTest.cpp
                PointPathComponentMapperTest.cpp
                VectorOfComponentMapperTest.cpp
                VectorOfMapperTest.cpp
                V3DMapperTest.cpp
)

add_executable(cow_instrument_test runner.cpp MockTypes.h ${TEST_FILES} ${TEST_FILES_MAPPERS} )

target_link_libraries(cow_instrument_test LINK_PRIVATE cow_instrument ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${GMOCK_MAIN_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})

add_test(cow_instrument_test cow_instrument_test)

