cmake_minimum_required(VERSION 2.6)

project(vallumd C)

list(APPEND ext_libs mosquitto)

add_executable(vallumd src/main.c src/mosquitto.c)
target_link_libraries(vallumd ${ext_libs})

install(TARGETS vallumd RUNTIME DESTINATION bin)
