cmake_minimum_required(VERSION 2.6)

project(vallumd C)

add_executable(vallumd src/main.c)

install(TARGETS vallumd RUNTIME DESTINATION bin)
