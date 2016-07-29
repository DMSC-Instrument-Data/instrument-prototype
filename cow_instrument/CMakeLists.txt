find_package ( Boost 1.54.0 COMPONENTS mpi REQUIRED )

find_package(MPI REQUIRED)

add_subdirectory(testing)

add_subdirectory(benchmark)

set ( SOURCE_FILES
                   CompositeComponent.cpp
                   DetectorComponent.cpp
                   InstrumentTree.cpp
                   MoveCommand.cpp
                   Node.cpp
                   NullComponent.cpp
                   ParabolicGuide.cpp
                   PathComponent.cpp
)

set ( INCLUDE_FILES
                   Bool.h
                   Command.h
                   Component.h
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
                   SourceSampleDetectorPathFactory.h
                   SpectrumInfo.h
                   Spectrum.h
                   VectorOf.h
                   V3D.h
)

add_library (cow_instrument SHARED ${SOURCE_FILES} ${INCLUDE_FILES})

target_include_directories (cow_instrument PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

