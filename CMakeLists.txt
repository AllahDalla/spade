cmake_minimum_required(VERSION 3.10)
project(spade C)

set(CMAKE_BUILD_TYPE Debug)

# Set debug flags for different compilers
if(MSVC)
    set(CMAKE_C_FLAGS_DEBUG "/Zi /Od /MDd")
    set(CMAKE_C_FLAGS_RELEASE "/O2 /MD")
else()
    set(CMAKE_C_FLAGS_DEBUG "-g -O0")
    set(CMAKE_C_FLAGS_RELEASE "-O2")
endif()

add_executable(spade spade.c spade.lexer.c spade.parser.c spade.symbol.c spade.semantic.c spade.ir.c spade.vm.c)
