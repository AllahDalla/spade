cmake_minimum_required(VERSION 3.10)
project(spade C)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")

add_executable(spade spade.c spade.lexer.c spade.parser.c spade.symbol.c spade.semantic.c spade.ir.c spade.vm.c)
