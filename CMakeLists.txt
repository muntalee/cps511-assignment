cmake_minimum_required(VERSION 3.5)
project(game VERSION 1.0)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# output file same name as project name
set(OUT ${CMAKE_PROJECT_NAME})

# search for all files in src folder
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# add files to executable
add_executable(${OUT} ${SOURCES})

# copy data folder to build directory
file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})

# include directories
target_include_directories(${OUT} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# glm
add_subdirectory(${CMAKE_SOURCE_DIR}/extern/glm)
target_compile_definitions(${OUT} PRIVATE GLM_ENABLE_EXPERIMENTAL)

# glew (static build)
set(GLEW_SRC ${CMAKE_SOURCE_DIR}/extern/glew/src/glew.c)
add_library(glew STATIC ${GLEW_SRC})
target_include_directories(glew PUBLIC ${CMAKE_SOURCE_DIR}/extern/glew/include)

# SOIL
add_subdirectory(${CMAKE_SOURCE_DIR}/extern/SOIL)
target_include_directories(${OUT} PRIVATE ${CMAKE_SOURCE_DIR}/extern/SOIL/inc/SOIL)

# freeglut
if (NOT APPLE)
    set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)

    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/freeglut)
    target_include_directories(${OUT} PRIVATE ${CMAKE_SOURCE_DIR}/extern/freeglut/include)
endif()

# make sure Windows builds know it's static
if (WIN32)
    target_compile_definitions(glew PUBLIC GLEW_STATIC)
endif()

# link libraries
find_package(OpenGL REQUIRED)
target_link_libraries(${OUT}
    PRIVATE
        glew
        glm
        OpenGL::GL
        soil
)

# platform-specific
if (WIN32)
    target_link_libraries(${OUT}
        PRIVATE
            freeglut_static
            glu32
            gdi32
            user32
            kernel32
    )
elseif (APPLE)
    target_link_libraries(${OUT}
        PRIVATE
            "-framework GLUT"
            "-framework OpenGL"
    )
    add_definitions(-DGL_SILENCE_DEPRECATION)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(${OUT}
        PRIVATE
            freeglut_static
            OpenGL::GL
            OpenGL::GLU
            m
            X11 Xrandr Xi Xxf86vm GLX OpenGL
    )
endif()
