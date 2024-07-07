set(CPPS
  decoder8.cpp
  decoder16.cpp
  movie_sound.cpp
  mve_audio.cpp
  mvelib.cpp
  mveplay.cpp
)

add_library(libmve STATIC ${CPPS})
target_link_libraries(libmve PRIVATE
  ddio
  
)
target_include_directories(libmve PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/libmve
  >
)
