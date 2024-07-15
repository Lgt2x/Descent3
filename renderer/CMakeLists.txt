set(HEADERS
  dyna_gl.h
  HardwareInternal.h
)
set(CPPS
  HardwareClipper.cpp
  HardwareDraw.cpp
  HardwareGlobalVars.cpp
  HardwareInstance.cpp
  HardwareOpenGL.cpp
  HardwareBaseGPU.cpp
  HardwarePoints.cpp
  HardwareSetup.cpp
  HardwareTransforms.cpp
  lnxscreenmode.cpp
)

# These are excluded.
#opengl.cpp
#renderer.cpp
#Direct3D.cpp

add_library(renderer STATIC ${HEADERS} ${CPPS})
target_link_libraries(renderer PRIVATE
  bitmap
  ddio
  mem
  misc
  $<$<PLATFORM_ID:Linux>:SDL2::SDL2>
)

if(EMSCRIPTEN)
  target_include_directories(renderer PUBLIC ${CMAKE_SOURCE_DIR})
  target_link_libraries(renderer PRIVATE GL)
endif()