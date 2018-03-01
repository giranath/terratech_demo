cmake_minimum_required(VERSION 3.9)
project(mmap_demo VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(terratech 0.5 REQUIRED)
find_package(sdl2 REQUIRED)
find_package(sdl2_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(PythonInterp REQUIRED)

# Generates GL3W files
execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/thirdparty/gl3w/gl3w_gen.py")

add_executable(mmap_demo
        "${CMAKE_BINARY_DIR}/src/gl3w.c"
        src/main.cpp
        src/sdl/sdl.hpp
        src/game.cpp
        src/game.hpp
        src/opengl/opengl.hpp
        src/opengl/shader.cpp
        src/opengl/shader.hpp
        src/opengl/program.cpp
        src/opengl/program.hpp
        src/opengl/bind.hpp
        src/opengl/vertex_array.cpp
        src/opengl/vertex_array.hpp
        src/opengl/buffer.cpp
        src/opengl/buffer.hpp
        src/sdl/poll_event_iterator.cpp
        src/sdl/poll_event_iterator.hpp
        src/sdl/window.cpp
        src/sdl/window.hpp
        src/sdl/context.hpp
        src/camera.cpp
        src/camera.hpp
        src/opengl/uniform.hpp)

target_include_directories(mmap_demo PUBLIC
        ${terratech_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        "${CMAKE_BINARY_DIR}/include")
target_link_libraries(mmap_demo
        ${terratech_LIBRARIES}
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${OPENGL_gl_LIBRARY}
        ${CMAKE_DL_LIBS})

target_compile_definitions(mmap_demo PRIVATE -DGAME_VERSION="${PROJECT_VERSION}")