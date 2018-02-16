cmake_minimum_required(VERSION 3.9)
project(mmap_demo VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)

find_package(mapgen 0.2 REQUIRED)
find_package(sdl2 REQUIRED)

add_executable(mmap_demo
        src/main.cpp src/wrapper/sdl.hpp src/wrapper/sdl.cpp src/game.cpp src/game.hpp)
target_include_directories(mmap_demo PUBLIC ${mapgen_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
target_link_libraries(mmap_demo ${mapgen_LIBRARIES} ${SDL2_LIBRARIES})
target_compile_definitions(mmap_demo PRIVATE -DGAME_VERSION="${PROJECT_VERSION}")