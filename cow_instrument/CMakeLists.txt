add_subdirectory(testing)


set ( SOURCE_FILES
                   CompositeComponent.cpp
                   Detector.cpp
                   InstrumentTree.cpp
                   MoveCommand.cpp
                   Node.cpp
                   NodeIterator.cpp
                   NullComponent.cpp                   
)

set ( INCLUDE_FILES
                   Command.h
                   Component.h
                   CompositeComponent.h
                   cow_ptr.h
                   Detector.h
                   InstrumentTree.h
                   MoveCommand.h
                   Node.h
                   NodeIterator.h
                   NullComponent.h
)

add_library (cow_instrument SHARED ${SOURCE_FILES} ${INCLUDE_FILES})

target_include_directories (cow_instrument PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

