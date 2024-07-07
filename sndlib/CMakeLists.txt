set(CPPS
  hlsoundlib.cpp
  sndrender.cpp
  soundload.cpp
  ddsoundload.cpp
  ssl_lib.cpp
  mixer.cpp
  sdlgeometry.cpp
  sdlsound.cpp
)

add_library(sndlib STATIC ${CPPS})
target_link_libraries(sndlib PRIVATE
  cfile
  ddio
  mem
  misc
  stream_audio
  SDL2-static SDL2main
)
target_include_directories(sndlib PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/sndlib
  >
)
