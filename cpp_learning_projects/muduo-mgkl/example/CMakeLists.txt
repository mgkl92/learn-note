file(GLOB EXAMPLE_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(testserver ${EXAMPLE_SRCS})

target_link_libraries(testserver muduo_core ${LIBS})

target_compile_options(testserver PRIVATE -std=c++11 -Wall)

set_target_properties(testserver PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

