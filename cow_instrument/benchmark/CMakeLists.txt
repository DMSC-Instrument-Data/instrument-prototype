# Auto-find correct threading library
find_package(Threads)

# Location of googletest (by default, expected adjacent to instrument-prototype)
get_filename_component( DEFAULT_GBENCH_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../benchmark" REALPATH )
get_filename_component( DEFAULT_GBENCH_BUILD_DIR "${CMAKE_SOURCE_DIR}/../benchmark" REALPATH )
set( GBENCH_SOURCE_DIR "${DEFAULT_GBENCH_SOURCE_DIR}" CACHE PATH "GoogleBenchmark top directory" )
set( GBENCH_BUILD_DIR "${DEFAULT_GBENCH_BUILD_DIR}" CACHE PATH "GoogleBenchmark build directory" )

# GBENCH config
get_filename_component( GBENCH_INCLUDE_DIR "${GBENCH_SOURCE_DIR}/include" REALPATH )
get_filename_component( GBENCH_LIBRARY "${GBENCH_BUILD_DIR}/src/libbenchmark.a" REALPATH )

# Validate directories
if( NOT EXISTS "${GBENCH_INCLUDE_DIR}/" )
    message( SEND_ERROR "GoogleBenchmark include directories missing. \nEnsure GBENCH_SOURCE_DIR is correctly set to GoogleBenchmark top directory." )
endif()

if( NOT EXISTS "${GBENCH_LIBRARY}" )
    message( SEND_ERROR "GoogleBenchmark library not found. \nEnsure GBENCH_BUILD_DIR is correctly set to GoogleBenchmark build directory." )
endif()

# Set include directories
include_directories(${GBENCH_INCLUDE_DIR})

set ( BENCH_FILES_SRC
                 InstrumentConstructionBenchmark.cpp
                 DetectorInfoConstructionBenchmark.cpp
                 DetectorInfoReadBenchmark.cpp
                 InstrumentSerializationBenchmark.cpp
                 DetectorInfoWriteTranslateBenchmark.cpp
                 DetectorInfoWriteRotateBenchmark.cpp
                 ScanningDetectorInfo.cpp
                 SpectrumInfoBenchmark.cpp
                 StandardInstrument.cpp
)

set ( BENCH_FILES_H
                 StandardInstrument.h
)

add_executable(cow_instrument_bench ${BENCH_FILES_SRC} ${BENCH_FILES_H})

target_link_libraries(cow_instrument_bench LINK_PUBLIC cow_instrument ${GBENCH_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

