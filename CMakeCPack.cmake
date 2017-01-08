# Packaging information
set(CPACK_GENERATOR "DEB;RPM")
set(CPACK_OUTPUT_FILE_PREFIX "build/")
set(CPACK_PACKAGE_CONTACT "Stijn Tintel <stijn@linux-ipv6.be>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "vallumd")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
SET(CPACK_PACKAGE_VERSION_MAJOR "${vallumd_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${vallumd_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${vallumd_VERSION_PATCH}")
set(CPACK_STRIP_FILES true)

configure_file("CMakeCPackOptions.cmake.in" "CMakeCPackOptions.cmake" @ONLY)
set(CPACK_PROJECT_CONFIG_FILE "CMakeCPackOptions.cmake")

include(CPack)
