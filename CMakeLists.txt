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

find_package(PkgConfig REQUIRED)
pkg_search_module(SYSTEMD QUIET "systemd")
if (SYSTEMD_FOUND)
    if ("${SYSTEMD_SYSTEM_UNIT_DIR}" STREQUAL "")
        execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} systemd
                        --variable=systemdsystemunitdir
                        OUTPUT_VARIABLE SYSTEMD_SYSTEM_UNIT_DIR
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
    set(UNIT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/init")
    install(FILES "${UNIT_DIR}/${PROJECT_NAME}.service"
            DESTINATION "${SYSTEMD_SYSTEM_UNIT_DIR}")
endif ()

install(TARGETS vallumd RUNTIME DESTINATION sbin)

include (CMakeCPack.cmake)
