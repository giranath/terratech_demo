cmake_minimum_required(VERSION 3.9)
project(mmap_demo VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(terratech 0.5 REQUIRED)
find_package(sdl2 REQUIRED)
find_package(sdl2_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(PythonInterp REQUIRED)

# Generates GL3W files
execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/thirdparty/gl3w/gl3w_gen.py")

# Download Json.hpp
file(DOWNLOAD https://github.com/nlohmann/json/releases/download/v3.1.1/json.hpp include/json/json.hpp)

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
        src/rendering/camera.cpp
        src/rendering/camera.hpp
        src/opengl/uniform.hpp
        src/crypto/rsa.cpp
        src/crypto/rsa.hpp
        src/world/world.cpp
        src/world/world.hpp
        src/world/world_generator.cpp
        src/world/world_generator.hpp
        src/world/world_chunk.cpp
        src/world/world_chunk.hpp
        src/rendering/chunk_renderer.cpp
        src/rendering/chunk_renderer.hpp
        src/control/all_commands.hpp
        src/control/command.hpp
        src/control/input_handler.hpp
        src/rendering/chunk_renderer.cpp
        src/rendering/chunk_renderer.hpp
        src/debug/profiler.hpp
        src/debug/profiler_administrator.hpp
        src/opengl/debug.cpp
        src/opengl/debug.hpp
        src/rendering/world_renderer.cpp
        src/rendering/world_renderer.hpp
        src/world/site.cpp
        src/world/site.hpp
        src/rendering/mesh.cpp
        src/rendering/mesh.hpp
        src/bounding_box.hpp
        src/opengl/texture.cpp
        src/opengl/texture.hpp
        src/sdl/istream_rwops.cpp
        src/sdl/istream_rwops.hpp)

target_include_directories(mmap_demo PUBLIC
        ${terratech_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${sdl2_image_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        "${CMAKE_BINARY_DIR}/include")
target_link_libraries(mmap_demo
        ${terratech_LIBRARIES}
        ${SDL2_LIBRARIES}
        ${sdl2_image_LIBRARIES}
        ${OPENGL_gl_LIBRARY}
        ${CMAKE_DL_LIBS})

target_compile_definitions(mmap_demo PRIVATE -DGAME_VERSION="${PROJECT_VERSION}")