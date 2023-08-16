cmake_minimum_required(VERSION 3.10)
project(GraphDraw)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# We will generate all the executable in the same location for simplicity
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

add_subdirectory("${CMAKE_SOURCE_DIR}/GRender" GRender)

add_library(GraphDraw STATIC "graphDraw.cpp")
target_link_libraries(GraphDraw PUBLIC GRender)

add_executable(Main "main.cpp")
target_link_libraries(Main PRIVATE GraphDraw)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set_target_properties(Main PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	set_target_properties(Main PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
endif()




