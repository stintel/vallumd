# SPDX-License-Identifier: GPL-3.0 */
# Copyright (C) 2017-2021  Stijn Tintel <stijn@linux-ipv6.be>

cmake_minimum_required(VERSION 2.8.12)

project(vallumd C)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 2)
set (VERSION_PATCH 0)

execute_process(COMMAND git describe --tags --dirty
		OUTPUT_VARIABLE GIT_DESCRIBE
		OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
execute_process(COMMAND git describe --abbrev=0
                OUTPUT_VARIABLE GIT_LAST_TAG
                OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

string(REGEX REPLACE "^${GIT_LAST_TAG}-" "" GIT_DESCRIBE_NOTAG "${GIT_DESCRIBE}")

if (NOT "${GIT_DESCRIBE}" STREQUAL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
    if ("${GIT_LAST_TAG}" VERSION_LESS
        "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
        string(REGEX REPLACE "^${GIT_LAST_TAG}-"
                "" VERSION_TWEAK "0-pre-${GIT_DESCRIBE_NOTAG}")
    else ()
	string(REGEX REPLACE
		"^${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-"
		"" VERSION_TWEAK "${GIT_DESCRIBE}")
    endif ()
endif ()


configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Werror -Wextra")

add_definitions(-D_GNU_SOURCE)

include_directories("${PROJECT_BINARY_DIR}")

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBIPSET QUIET libipset)

option (WITH_LIBIPSET_V6_COMPAT "Enable compatibility with libipset version 6" OFF)
if ((LIBIPSET_FOUND AND LIBIPSET_VERSION VERSION_LESS 7) OR WITH_LIBIPSET_V6_COMPAT)
    add_definitions("-DWITH_LIBIPSET_V6_COMPAT")
endif ()

option (WITH_TLS "Enable TLS support" ON)
if (${WITH_TLS} STREQUAL ON)
    pkg_search_module(LIBSSL QUIET "libssl")
    if (LIBSSL_FOUND)
        add_definitions("-DWITH_TLS")
        list(APPEND ext_libs ssl)
    else ()
        message(FATAL_ERROR "TLS enabled but LibreSSL or OpenSSL not found!")
    endif ()
endif ()

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
