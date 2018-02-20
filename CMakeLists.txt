cmake_minimum_required(VERSION 3.9)
project(mmap_demo VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(mapgen 0.2 REQUIRED)
find_package(sdl2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PythonInterp REQUIRED)

# Generates GL3W files
execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/thirdparty/gl3w/gl3w_gen.py")

add_executable(mmap_demo
        "${CMAKE_BINARY_DIR}/src/gl3w.c"
        src/main.cpp
        src/wrapper/sdl.hpp
        src/wrapper/sdl.cpp
        src/game.cpp
        src/game.hpp
        src/opengl/opengl.hpp
        src/opengl/shader.cpp
        src/opengl/shader.hpp src/opengl/program.cpp src/opengl/program.hpp src/opengl/bind.hpp src/opengl/vertex_array.cpp src/opengl/vertex_array.hpp src/opengl/buffer.cpp src/opengl/buffer.hpp)

target_include_directories(mmap_demo PUBLIC ${mapgen_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} "${CMAKE_BINARY_DIR}/include")
target_link_libraries(mmap_demo ${mapgen_LIBRARIES} ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})
target_compile_definitions(mmap_demo PRIVATE -DGAME_VERSION="${PROJECT_VERSION}")