cmake_minimum_required(VERSION 2.8.12)

project(vallumd C)

set (vallumd_VERSION_MAJOR 0)
set (vallumd_VERSION_MINOR 0)
set (vallumd_VERSION_PATCH 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Werror -Wextra")

add_definitions(-D_GNU_SOURCE)

include_directories("${PROJECT_BINARY_DIR}")

list(APPEND ext_libs ipset)
list(APPEND ext_libs mosquitto)

add_executable(vallumd src/main.c src/mosquitto.c src/ipset.c)
target_link_libraries(vallumd ${ext_libs})

if (EXISTS "/etc/debian_version")
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/conf/${PROJECT_NAME}"
            DESTINATION "/etc/default")
elseif (EXISTS "/etc/gentoo-release")
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/conf/${PROJECT_NAME}"
            DESTINATION "/etc/conf.d")
elseif (EXISTS "/etc/redhat-release")
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/conf/${PROJECT_NAME}"
            DESTINATION "/etc/sysconfig")
endif ()

find_package(PkgConfig REQUIRED)
pkg_search_module(OPENRC QUIET "openrc")
if (OPENRC_FOUND)
    install(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/init/${PROJECT_NAME}.openrc"
            DESTINATION "/etc/init.d" RENAME "${PROJECT_NAME}")
endif ()
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
execute_process(COMMAND /sbin/init --version
                OUTPUT_VARIABLE INIT_VERSION ERROR_QUIET)
if ("${INIT_VERSION}" MATCHES "upstart")
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/init/${PROJECT_NAME}.upstart"
            DESTINATION "/etc/init" RENAME "${PROJECT_NAME}.conf")
endif ()

install(TARGETS vallumd RUNTIME DESTINATION sbin)

include (CMakeCPack.cmake)
