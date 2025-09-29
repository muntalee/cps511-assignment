cmake_minimum_required(VERSION 3.16)
project(game VERSION 1.0)

# output file same name as project name
set(OUT ${CMAKE_PROJECT_NAME})

# search for all files in src folder
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# add files to executable
add_executable(${OUT} ${SOURCES})

# include directories
target_include_directories(${OUT} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# glm
add_subdirectory(${CMAKE_SOURCE_DIR}/extern/glm)

# glew (static build)
set(GLEW_SRC ${CMAKE_SOURCE_DIR}/extern/glew/src/glew.c)
add_library(glew STATIC ${GLEW_SRC})
target_include_directories(glew PUBLIC ${CMAKE_SOURCE_DIR}/extern/glew/include)

# Windows/Linux → FreeGLUT
if (NOT APPLE)
    set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "" FORCE)      # no demos
    set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # build static
    set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)

    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/freeglut)
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
)

if (WIN32)
    # Required for wgl* functions
    target_link_libraries(${OUT}
        PRIVATE
            gdi32
            user32
    )
endif()

if(APPLE)
    # Use system GLUT (native)
    target_link_libraries(${OUT}
        PRIVATE
            "-framework GLUT"
            "-framework OpenGL"
    )
else()
    # Linux/Windows link with FreeGLUT
    target_link_libraries(${OUT}
        PRIVATE
            freeglut_static
    )
endif()

