# SPDX-License-Identifier: GPL-3.0 */
# Copyright (C) 2017-2021  Stijn Tintel <stijn@linux-ipv6.be>

# Packaging information
set(CPACK_GENERATOR "DEB;RPM")
set(CPACK_OUTPUT_FILE_PREFIX "build/")
set(CPACK_PACKAGE_CONTACT "Stijn Tintel <stijn@linux-ipv6.be>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "vallumd")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
SET(CPACK_PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(CPACK_STRIP_FILES true)

if(VERSION_TWEAK)
    set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}-${VERSION_TWEAK}")
endif()

configure_file("CMakeCPackOptions.cmake.in" "CMakeCPackOptions.cmake" @ONLY)
set(CPACK_PROJECT_CONFIG_FILE "CMakeCPackOptions.cmake")

include(CPack)
