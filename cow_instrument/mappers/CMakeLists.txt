
if(${Boost_VERSION} LESS 105700)
    add_definitions(-DCUSTOM_STD_ARRAY_SERIALIZATION)
endif()

set ( SOURCE_FILES
                   DetectorComponentMapper.cpp
)


set ( INCLUDE_FILES
                   ComponentIdTypeMapper.h
                   DetectorComponentMapper.h
                   DetectorIdTypeMapper.h
                   IntToTypeMapper.h
                   V3DMapper.h
)

# Make mappers a static library
add_library (cow_mappers STATIC ${SOURCE_FILES} ${INCLUDE_FILES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

set_target_properties(cow_mappers PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(cow_mappers LINK_PRIVATE ${Boost_LIBRARIES} )

target_include_directories (cow_mappers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
