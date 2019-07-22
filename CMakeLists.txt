cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("Klassifier")

file(GLOB_RECURSE sources      src/*.cpp src/*.h)

add_executable(klassifier ${sources} ${data})

#target_compile_options(klassifier PUBLIC -std=c++1y -Wall -Wfloat-conversion)

if(WIN32)
    target_link_libraries(klassifier shlwapi)
endif()
