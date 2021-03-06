cmake_minimum_required(VERSION 3.9)
project(mmap_demo VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif(MSVC)

option(DISABLE_PROFILER "Disable profiling when ON" OFF)
option(ENABLE_TOOLS "Enable tools compilation" OFF)
option(DISABLE_SERVER "Disable server" OFF)
option(ENABLE_CRYPTO "Enables cryptography" ON)

find_package(terratech 0.6.0 REQUIRED)
find_package(sdl2 REQUIRED)
find_package(sdl2_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(PythonInterp REQUIRED)
find_package(Threads REQUIRED)
find_package(sdl2_net REQUIRED)

if(ENABLE_CRYPTO)
find_package(Crypto++ REQUIRED)
endif()

if(WIN32)
    set(SOCKET_LIBRARIES wsock32 ws2_32)
else()
    set(SOCKET_LIBRARIES)
endif()

# Generates GL3W files
execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/thirdparty/gl3w/gl3w_gen.py")

file(COPY "${CMAKE_SOURCE_DIR}/asset" DESTINATION "${CMAKE_BINARY_DIR}")

set(SOURCE_CRYPTO "")
if(ENABLE_CRYPTO)
    set(SOURCE_CRYPTO
        src/common/crypto/aes.hpp
        src/common/crypto/iterator_sink.hpp
        src/common/crypto/rsa.hpp)
endif()

add_library(common
        ${SOURCE_CRYPTO}

        src/common/bounding_box.hpp

        src/common/world/world.cpp
        src/common/world/world.hpp
        src/common/world/world_generator.cpp
        src/common/world/world_generator.hpp
        src/common/world/world_chunk.cpp
        src/common/world/world_chunk.hpp
        src/common/world/site.cpp
        src/common/world/site.hpp
        src/common/world/infinite_world.cpp
        src/common/world/biome_type.hpp
        src/common/world/constants.hpp
        src/common/world/visibility_map.cpp
        src/common/world/visibility_map.hpp

        src/common/actor/actor.hpp
        src/common/actor/actor.cpp
        src/common/actor/base_unit.hpp
        src/common/actor/base_unit.cpp
        src/common/actor/building.hpp
        src/common/actor/ressource.hpp
        src/common/actor/ressource_type.hpp
        src/common/actor/target_handle.hpp
        src/common/actor/target_handle.cpp
        src/common/actor/unit.hpp
        src/common/actor/unit.cpp
        src/common/actor/unit_flyweight.hpp
        src/common/actor/unit_manager.hpp
        src/common/actor/unit_manager.cpp
        src/common/actor/unit_flyweight.cpp
        src/common/actor/actor_arena.hpp

        src/common/async/task_executor.cpp
        src/common/async/task_executor.hpp
        src/common/async/task.hpp
        src/common/async/spinlock.cpp
        src/common/async/spinlock.hpp
        src/common/async/event.hpp

        src/common/time/clock.hpp

        src/common/collision/circle_shape.cpp
        src/common/collision/circle_shape.hpp
        src/common/collision/aabb_shape.cpp
        src/common/collision/aabb_shape.hpp
        src/common/collision/collision_detector.cpp
        src/common/collision/collision_detector.hpp

        src/common/datadriven/virtual_texture_list_record.hpp
        src/common/datadriven/virtual_texture_list_record.cpp
        src/common/datadriven/shader_list_record.cpp
        src/common/datadriven/shader_list_record.hpp
        src/common/datadriven/texture_list_record.cpp
        src/common/datadriven/texture_list_record.hpp
        src/common/datadriven/data_list.hpp

        src/common/networking/packet.hpp
        src/common/networking/packet.cpp
        src/common/networking/tcp_socket.cpp
        src/common/networking/tcp_socket.hpp
        src/common/networking/socket_set.cpp
        src/common/networking/socket_set.hpp
        src/common/networking/world_chunk.hpp
        src/common/networking/world_map.hpp
        src/common/networking/networking_constant.hpp
        src/common/networking/network_manager.cpp
        src/common/networking/network_manager.hpp
        src/common/networking/update_target.hpp
        src/common/networking/update_target.cpp
        src/common/networking/player_init.hpp
        src/common/networking/player_init.cpp

        src/common/game/base_game.cpp 
        src/common/game/base_game.hpp

        src/common/memory/allocator_traits.hpp
        src/common/memory/malloc_allocator.cpp
        src/common/memory/malloc_allocator.hpp
        src/common/memory/frame_allocator.cpp
        src/common/memory/frame_allocator.hpp
        src/common/memory/heap_allocator.cpp
        src/common/memory/heap_allocator.hpp
        src/common/memory/allocator.hpp
		src/common/memory/static_vector.hpp
        src/common/util/vec_hash.hpp

        src/common/task/update_player_visibility.cpp
        src/common/task/update_player_visibility.hpp
        src/common/task/update_units.cpp
        src/common/task/update_units.hpp
        )
target_include_directories(common PUBLIC
        "${CMAKE_SOURCE_DIR}/thirdparty"
		${CRYPTO++_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIRS}
        ${sdl2_net_INCLUDE_DIRS}
        ${terratech_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS})
if(NOT ENABLE_CRYPTO)
    target_compile_definitions(common PRIVATE -DNCRYPTO)
endif()

add_executable(mmap_demo
        "${CMAKE_BINARY_DIR}/src/gl3w.c"
        src/client/main.cpp
        src/client/game.cpp
        src/client/game.hpp

        src/client/opengl/opengl.hpp
        src/client/opengl/shader.cpp
        src/client/opengl/shader.hpp
        src/client/opengl/program.cpp
        src/client/opengl/program.hpp
        src/client/opengl/bind.hpp
        src/client/opengl/vertex_array.cpp
        src/client/opengl/vertex_array.hpp
        src/client/opengl/buffer.cpp
        src/client/opengl/buffer.hpp
        src/client/opengl/uniform.hpp
        src/client/opengl/debug.cpp
        src/client/opengl/debug.hpp
        src/client/opengl/texture.cpp
        src/client/opengl/texture.hpp
        src/client/opengl/frame_buffer.cpp
        src/client/opengl/frame_buffer.hpp
        src/client/opengl/render_buffer.cpp
        src/client/opengl/render_buffer.hpp

        src/client/sdl/sdl.hpp
        src/client/sdl/poll_event_iterator.cpp
        src/client/sdl/poll_event_iterator.hpp
        src/client/sdl/window.cpp
        src/client/sdl/window.hpp
        src/client/sdl/context.hpp
        src/client/sdl/istream_rwops.cpp
        src/client/sdl/istream_rwops.hpp

        src/client/rendering/camera.cpp
        src/client/rendering/camera.hpp
        src/client/rendering/chunk_renderer.cpp
        src/client/rendering/chunk_renderer.hpp
        src/client/rendering/world_renderer.cpp
        src/client/rendering/world_renderer.hpp
        src/client/rendering/mesh.cpp
        src/client/rendering/mesh.hpp
        src/client/rendering/rendering_manager.cpp
        src/client/rendering/rendering_manager.hpp
        src/client/rendering/virtual_texture.cpp
        src/client/rendering/virtual_texture.hpp

        src/client/control/all_commands.hpp
        src/client/control/command.hpp
        src/client/control/key_input_handler.hpp
        src/client/control/key_input_handler.cpp
        src/client/control/camera_commands.hpp
        src/client/control/change_unit_command.cpp
        src/client/control/change_unit_command.hpp
        src/client/control/generic_command.hpp

        src/client/debug/profiler.hpp
        src/client/debug/profiler_administrator.hpp

        src/client/constant/rendering.hpp src/client/control/mouse_input_handler.cpp src/client/control/mouse_input_handler.hpp src/client/control/event_manager.cpp src/client/control/event_manager.hpp)

target_include_directories(mmap_demo PUBLIC
        ${terratech_INCLUDE_DIRS}
        ${CRYPTO++_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIRS}
        ${sdl2_image_INCLUDE_DIRS}
        ${sdl2_net_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        "${CMAKE_SOURCE_DIR}/thirdparty"
        "${CMAKE_BINARY_DIR}/include")
target_link_libraries(mmap_demo
        common
        ${CRYPTO++_LIBRARIES}
        ${SOCKET_LIBRARIES}
        ${terratech_LIBRARIES}
        ${SDL2_LIBRARIES}
        ${sdl2_image_LIBRARIES}
        ${sdl2_net_LIBRARIES}
        ${OPENGL_gl_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
        ${CMAKE_DL_LIBS})
set_target_properties(mmap_demo PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_compile_definitions(mmap_demo PRIVATE -DGAME_VERSION="${PROJECT_VERSION}")

if(NOT ENABLE_CRYPTO)
    target_compile_definitions(mmap_demo PRIVATE -DNCRYPTO)
endif()

if (DISABLE_PROFILER)
    target_compile_definitions(mmap_demo PRIVATE -DNPROFILER)
endif ()

if (ENABLE_TOOLS)
    add_subdirectory(tool)
endif ()

if (NOT DISABLE_SERVER)
    add_executable(server
            src/server/main.cpp
            src/server/authoritative_game.cpp
            src/server/authoritative_game.hpp
            src/server/server_unit_manager.cpp
            src/server/server_unit_manager.hpp
            src/server/client.cpp
            src/server/client.hpp)
    target_include_directories(server PRIVATE
            ${terratech_INCLUDE_DIRS}
            ${CRYPTO++_INCLUDE_DIR}
            ${SDL2_INCLUDE_DIRS}
            ${sdl2_net_INCLUDE_DIRS}
            ${GLM_INCLUDE_DIRS}
            "${CMAKE_SOURCE_DIR}/thirdparty")
    target_link_libraries(server
            common
            ${CRYPTO++_LIBRARIES}
            ${SOCKET_LIBRARIES}
            ${terratech_LIBRARIES}
            ${SDL2_LIBRARIES}
            ${sdl2_net_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT}
            ${CMAKE_DL_LIBS})
    target_compile_definitions(server PRIVATE -DGAME_SERVER)
	
	if(NOT ENABLE_CRYPTO)
    target_compile_definitions(server PRIVATE -DNCRYPTO)
	endif()
	
endif()