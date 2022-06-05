# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
project ("h264p")

# Include sub-projects.
add_subdirectory ("h264p")
add_subdirectory ("libh264p")

# Test cases
enable_testing()

set(args ${PROJECT_SOURCE_DIR}/sample/svt_testset_420_720p50_klved_4774.h264)

add_test(NAME Parse
	COMMAND h264p ${args})