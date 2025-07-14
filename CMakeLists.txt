cmake_minimum_required(VERSION 4.0)
project(spade)

add_executable(spade spade.c spade.lexer.c spade.parser.c spade.symbol.c spade.semantic.c spade.ir.c spade.vm.c)
