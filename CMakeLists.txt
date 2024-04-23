cmake_minimum_required(VERSION 3.20)
project(GraphDraw)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}")


add_subdirectory("${CMAKE_SOURCE_DIR}/GRender" GRender)

add_library(GraphDraw STATIC "graphDraw.cpp")
target_link_libraries(GraphDraw PUBLIC GRender)

add_executable(Main "main.cpp")
target_link_libraries(Main PRIVATE GraphDraw)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set_target_properties(Main PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	set_target_properties(Main PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
endif()




