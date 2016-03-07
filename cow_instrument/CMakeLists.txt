

add_library (cow_instrument SHARED Instrument.cpp)

target_include_directories (cow_instrument PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

