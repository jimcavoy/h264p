# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)
project (h264p
	VERSION 1.0.0
	DESCRIPTION "H.264 Parser library and test application, avc2json"
	LANGUAGES CXX)

# Include sub-projects.
add_subdirectory (h264p)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS json)

add_executable(avc2json)

target_sources(avc2json
  PRIVATE
    src/h264p.cpp
    src/MDPMParser.cpp
    src/MDPMParser.h
    src/SEIParser.cpp
    src/SEIParser.h
    src/TestH264Parser.cpp
    src/TestH264Parser.h
)

target_include_directories(avc2json
  PRIVATE
    ${Boost_INCLUDE_DIRS})

target_link_libraries(avc2json 
  PRIVATE
    h264p
    Boost::json
)

# Test cases
enable_testing()

add_test(NAME Parse
  COMMAND avc2json ${PROJECT_SOURCE_DIR}/sample/svt_testset_420_720p50_klved_4774.h264
)
