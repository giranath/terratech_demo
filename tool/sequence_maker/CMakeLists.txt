cmake_minimum_required(VERSION 3.0)

project(SequenceMaker)

find_package(SFML COMPONENTS graphics window system REQUIRED)

add_executable(sequencemaker
        SequenceMaker3000.cpp)
target_include_directories(sequencemaker PUBLIC ${SFML_INCLUDE_DIR})
target_link_libraries(sequencemaker ${SFML_LIBRARIES})