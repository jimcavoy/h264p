# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)
project (h264p
	VERSION 1.0.0
	DESCRIPTION "H.264 Parser library and test application"
	LANGUAGES CXX)

# Include sub-projects.
add_subdirectory (h264p)

include_directories(../loki-lib/include)
include_directories(../cajun-jsonapi/include/cajun)

add_executable(h264ptest)

file(GLOB SRC_LIST src/*.cpp)

target_sources(h264ptest PRIVATE ${SRC_LIST})

target_link_libraries(h264ptest PRIVATE h264p)

# Test cases
enable_testing()

set(args ${PROJECT_SOURCE_DIR}/sample/svt_testset_420_720p50_klved_4774.h264)

add_test(NAME Parse
	COMMAND h264ptest ${args})