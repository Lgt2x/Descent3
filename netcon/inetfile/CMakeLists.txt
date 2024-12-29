set(HEADERS)

set(CPPS
  CFtp.cpp
  Chttpget.cpp
  inetgetfile.cpp)

add_library(inetfile STATIC ${HEADERS} ${CPPS})
target_link_libraries(inetfile PRIVATE
  ddio
  mem
  SDL3::SDL3
)
target_include_directories(inetfile PRIVATE ${SDL3_INCLUDE_DIRS})

