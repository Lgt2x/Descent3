set(CMAKE_FOLDER "third_party")

add_subdirectory(libacm)
add_subdirectory(stb)

message("USING BUNDLED SDL")
if(NOT DEFINED USING_VCPKG AND NOT USE_EXTERNAL_PLOG)
  if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/plog/CMakeLists.txt)
    message(FATAL_ERROR "plog third-party directory could not be found. "
      "Please run 'git submodule update --init --recursive' in the source directory.")
  endif()
  add_subdirectory(plog)
endif()

# VCPKG's port of SDL3 requires dbus, but dbus compilation fails when cross-compiling (vcpkg issue #40031),
# so we use the bundled version of SDL3 in this case, which does not need dbus by default.
if ((NOT DEFINED USING_VCPKG) OR CMAKE_CROSSCOMPILING)
  if (NOT USE_EXTERNAL_SDL)
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/SDL/CMakeLists.txt)
    message(FATAL_ERROR "SDL third-party directory could not be found. "
      "Please run 'git submodule update --init --recursive' in the source directory.")
    endif()
    add_subdirectory(SDL)
  endif()
endif()