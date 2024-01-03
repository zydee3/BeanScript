cmake_minimum_required(VERSION 3.22)
project(beanscript C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(beanscript src/main.c  src/keyboard/keyboard.c src/keyboard/keyboard.h src/keyboard/keycodes.h src/keyboard/keycodes.c src/utility/uthash.h src/parser/instruction.c src/parser/instruction.h src/utility/str_list.c src/utility/str_list.h src/utility/utility.c src/utility/utility.h src/main.h src/parser/parser.c src/parser/parser.h src/parser/lexer.c src/parser/lexer.h src/utility/str_bucket.c src/utility/str_bucket.h src/runtime.c src/runtime.h src/utility/str_timestamp_queue.c src/utility/str_timestamp_queue.h
        src/scheduler/routine.c
        src/scheduler/routine.h
        src/scheduler/waitlist.c
        src/scheduler/waitlist.h)

if (WIN32)
    target_link_libraries(beanscript ${CMAKE_CURRENT_SOURCE_DIR}/lib/interception.dll)
endif (WIN32)