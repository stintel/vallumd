cmake_minimum_required(VERSION 2.6)

project(vallumd C)

set (vallumd_VERSION_MAJOR 0)
set (vallumd_VERSION_MINOR 0)
set (vallumd_VERSION_PATCH 0)

list(APPEND ext_libs ipset)
list(APPEND ext_libs mosquitto)

add_executable(vallumd src/main.c src/mosquitto.c src/ipset.c)
target_link_libraries(vallumd ${ext_libs})

install(TARGETS vallumd RUNTIME DESTINATION sbin)
