set ( SOURCE_FILES
                   CompositeComponentMapper.cpp
                   DetectorComponentMapper.cpp
                   ParabolicGuideMapper.cpp
                   V3DMapper.cpp
)


set ( INCLUDE_FILES
                   ComponentIdTypeMapper.h
                   CompositeComponentMapper.h
                   ComponentMapper.h
                   ComponentMapperFactory.h
                   DetectorComponentMapper.h
                   DetectorIdTypeMapper.h
                   IntToTypeMapper.h
                   ParabolicGuideMapper.h
                   PointPathComponentMapper.h
                   PointSourceMapper.h
                   PointSampleMapper.h
                   PolymorphicSerializer.h
                   SingleItemMapper.h
                   V3DMapper.h
                   VectorOfMapper.h
                   VectorOfComponentMapper.h
)

# Make mappers a static library
add_library (cow_mappers STATIC ${SOURCE_FILES} ${INCLUDE_FILES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

set_target_properties(cow_mappers PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(cow_mappers LINK_PRIVATE ${Boost_LIBRARIES} )

target_include_directories (cow_mappers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${EIGEN3_INCLUDE_DIR})
