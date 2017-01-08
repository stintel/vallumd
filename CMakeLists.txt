cmake_minimum_required(VERSION 2.8.12)

project(vallumd C)

set (vallumd_VERSION_MAJOR 0)
set (vallumd_VERSION_MINOR 0)
set (vallumd_VERSION_PATCH 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra")

include_directories("${PROJECT_BINARY_DIR}")

list(APPEND ext_libs ipset)
list(APPEND ext_libs mosquitto)

add_executable(vallumd src/main.c src/mosquitto.c src/ipset.c)
target_link_libraries(vallumd ${ext_libs})

install(TARGETS vallumd RUNTIME DESTINATION sbin)

include (CMakeCPack.cmake)
