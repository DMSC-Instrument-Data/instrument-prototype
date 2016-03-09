add_subdirectory(testing)


set ( SOURCE_FILES Instrument.cpp 
                   Detector.cpp
                   NullComponent.cpp
                   Node.cpp
)

set ( INCLUDE_FILES Instrument.h
                   Detector.h
                   Command.h
                   Component.h
                   NullComponent.h
                   cow_ptr.h
                   Node.h
)

add_library (cow_instrument SHARED ${SOURCE_FILES} ${INCLUDE_FILES})

target_include_directories (cow_instrument PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

