#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "jansson::jansson" for configuration ""
set_property(TARGET jansson::jansson APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(jansson::jansson PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libjansson.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS jansson::jansson )
list(APPEND _IMPORT_CHECK_FILES_FOR_jansson::jansson "${_IMPORT_PREFIX}/lib/libjansson.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
