set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package ( Boost 1.54.0 COMPONENTS mpi serialization REQUIRED )

find_package(Eigen3 3.1.2 REQUIRED)


if(${Boost_VERSION} LESS 105700)
    message(STATUS "Add serialization overloads. Boost version ${Boost_VERSION}")
    add_definitions(-DLEGACY_BOOST_SERIALIZATION)
endif()

find_package(MPI REQUIRED)

add_subdirectory(testing)

add_subdirectory(benchmark)

add_subdirectory(mappers)

set ( SOURCE_FILES
                   CompositeComponent.cpp
                   DetectorComponent.cpp
                   InstrumentTree.cpp
                   MoveCommand.cpp
                   Node.cpp
                   NullComponent.cpp
                   ParabolicGuide.cpp
                   PathComponent.cpp
                   RotateCommand.cpp
)


set ( INCLUDE_FILES
                   Bool.h
                   Command.h
                   Component.h
                   ComponentVisitor.h
                   CompositeComponent.h
                   cow_ptr.h
                   Detector.h
                   DetectorComponent.h
                   DetectorInfo.h
                   FixedLengthVector.h
                   IdType.h
                   InstrumentTree.h
                   IntToType.h
                   L1s.h
                   L2s.h
                   MaskFlags.h
                   MonitorFlags.h
                   MoveCommand.h
                   Node.h
                   NullComponent.h
                   ParabolicGuide.h
                   Path.h
                   PathComponent.h
                   PathFactory.h
                   PointPathComponent.h
                   PointSample.h
                   PointSource.h
                   RotateCommand.h
                   SourceSampleDetectorPathFactory.h
                   SpectrumInfo.h
                   Spectrum.h
                   VectorOf.h
)



add_library (cow_instrument SHARED ${SOURCE_FILES} ${INCLUDE_FILES})

target_link_libraries(cow_instrument LINK_PUBLIC cow_mappers)

target_include_directories (cow_instrument PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/mappers  ${EIGEN3_INCLUDE_DIR})

