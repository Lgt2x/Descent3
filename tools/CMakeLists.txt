project(HogMaker LANGUAGES CXX)

add_executable(
	HogMakerBin
	HogMaker/HogFormat.cpp
	HogMaker/HogMaker.cpp
)
add_dependencies(HogMakerBin get_git_hash)
target_include_directories(HogMakerBin PRIVATE ${PROJECT_SOURCE_DIR}/../lib ${PROJECT_BINARY_DIR}/lib)
