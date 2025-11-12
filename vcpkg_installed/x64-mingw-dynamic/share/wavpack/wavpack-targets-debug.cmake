#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "WavPack::WavPack" for configuration "Debug"
set_property(TARGET WavPack::WavPack APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(WavPack::WavPack PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/libwavpack.dll.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/libwavpack-1.dll"
  )

list(APPEND _cmake_import_check_targets WavPack::WavPack )
list(APPEND _cmake_import_check_files_for_WavPack::WavPack "${_IMPORT_PREFIX}/debug/lib/libwavpack.dll.a" "${_IMPORT_PREFIX}/debug/bin/libwavpack-1.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
