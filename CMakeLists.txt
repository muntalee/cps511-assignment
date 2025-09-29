cmake_minimum_required(VERSION 3.16)
project(game VERSION 1.0)

# output file same name as project name
set(OUT ${CMAKE_PROJECT_NAME})

# search for all files in src folder
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# add files to executable
add_executable(${OUT} ${SOURCES})

# add external libraries
add_subdirectory(${CMAKE_SOURCE_DIR}/extern/glfw)

# include directories
target_include_directories(${OUT} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# link libraries
find_package(OpenGL REQUIRED)

target_link_libraries(${OUT} PRIVATE glfw OpenGL::GL)
