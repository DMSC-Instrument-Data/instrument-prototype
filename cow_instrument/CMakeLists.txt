set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package ( Boost 1.54.0 COMPONENTS serialization REQUIRED )

find_package(Eigen3 3.1.2 REQUIRED)


if(${Boost_VERSION} LESS 105700)
    message(STATUS "Add serialization overloads. Boost version ${Boost_VERSION}")
    add_definitions(-DLEGACY_BOOST_SERIALIZATION)
endif()

add_subdirectory(testing)

add_subdirectory(benchmark)

add_subdirectory(mappers)

set ( SOURCE_FILES
                   CompositeComponent.cpp
                   ComponentProxy.cpp
                   DetectorComponent.cpp
                   FlatTree.cpp
                   LinkedTreeParser.cpp
                   NullComponent.cpp
                   ParabolicGuide.cpp
                   PathComponent.cpp
                   ScanTime.cpp
)


set ( INCLUDE_FILES
                   AssemblyInfo.h
                   Bool.h
                   Component.h
                   ComponentInfo.h
                   ComponentProxy.h
                   ComponentVisitor.h
                   CompositeComponent.h
                   cow_ptr.h
                   Detector.h
                   DetectorComponent.h
                   DetectorInfo.h
                   FixedLengthVector.h
                   IdType.h
                   IndexTranslator.h
                   FlatTree.h
                   IntToType.h
                   L1s.h
                   L2s.h
                   LinkedTreeParser.h
                   MaskFlags.h
                   MonitorFlags.h
                   NullComponent.h
                   ParabolicGuide.h
                   Path.h
                   PathComponent.h
                   PathComponentInfo.h
                   PathFactory.h
                   PointPathComponent.h
                   PointSample.h
                   PointSource.h
                   ScanTime.h
                   SourceSampleDetectorPathFactory.h
                   SpectrumInfo.h
                   Spectrum.h
                   VectorOf.h
)



add_library (cow_instrument SHARED ${SOURCE_FILES} ${INCLUDE_FILES})

target_link_libraries(cow_instrument LINK_PUBLIC cow_mappers)

target_include_directories (cow_instrument PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/mappers  ${EIGEN3_INCLUDE_DIR})

