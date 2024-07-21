set(CPPS
  chrono_timer.cpp
  ddio.cpp
  file.cpp
  key.cpp
  lnxforcefeedback.cpp
  lnxio.cpp
  sdljoy.cpp
  lnxkey.cpp
  lnxkey_null.cpp
  lnxmouse.cpp
  lnxkey_sdl.cpp
  $<$<PLATFORM_ID:Darwin,Linux,Emscripten>:
    lnxfile.cpp
  >
  $<$<PLATFORM_ID:Windows>:
    winfile.cpp
  >
)
add_library(ddio STATIC ${CPPS})
target_link_libraries(ddio PRIVATE
  $<$<PLATFORM_ID:Linux>:$<$<PLATFORM_ID:Linux>:SDL2::SDL2>>
  ddebug
  mem
  misc
)
target_include_directories(ddio PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/ddio
  >
)
