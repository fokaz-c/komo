cmake_minimum_required(VERSION 3.16)
project(fsm-engine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(fsm
    src/fsm.c
    src/event_queue.c
    src/fsm_debug.c
)

target_include_directories(fsm PUBLIC src)

target_compile_options(fsm PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_subdirectory(examples/alarm)
