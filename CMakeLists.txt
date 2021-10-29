cmake_minimum_required(VERSION 3.21)

project(6502Emu)

find_package(Curses)

set(SOURCES src/main.c
            src/cpu.c
            src/bus.c
            src/opcode.c)

add_executable(6502Emu ${SOURCES})

target_include_directories(6502Emu PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(6502Emu ${CURSES_LIBRARIES})
