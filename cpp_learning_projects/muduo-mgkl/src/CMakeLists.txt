file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(muduo_core SHARED ${SRC_FILES})

target_include_directories(muduo_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
