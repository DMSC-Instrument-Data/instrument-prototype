add_subdirectory(testing)
add_subdirectory(benchmark)

set ( SOURCE_FILES
                   CompositeComponent.cpp
                   DetectorComponent.cpp
                   InstrumentTree.cpp
                   MoveCommand.cpp
                   Node.cpp
                   NullComponent.cpp                   
)

set ( INCLUDE_FILES
                   Command.h
                   Component.h
                   CompositeComponent.h
                   cow_ptr.h
                   Detector.h
                   DetectorComponent.h
                   IdType.h
                   InstrumentTree.h
                   MoveCommand.h
                   Node.h
                   NullComponent.h
                   IntToType.h
)

add_library (cow_instrument SHARED ${SOURCE_FILES} ${INCLUDE_FILES})

target_include_directories (cow_instrument PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

