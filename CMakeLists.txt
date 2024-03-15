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

# include the module `ExternalProject`
include(ExternalProject)

# Add an external project from a downloaded source archive
ExternalProject_Add(
  cajun
  URL https://github.com/cajun-jsonapi/cajun-jsonapi/archive/refs/tags/2.1.1.tar.gz
  URL_HASH MD5=d84903f5243e4598df3929d421db399a
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(cajun SOURCE_DIR)

set(CAJUN_DIR "${SOURCE_DIR}")

include_directories(${CAJUN_DIR}/include/cajun)

add_executable(avc2json)

target_sources(avc2json PRIVATE
    src/h264p.cpp
    src/MDPMParser.cpp
    src/MDPMParser.h
    src/SEIParser.cpp
    src/SEIParser.h
    src/TestH264Parser.cpp
    src/TestH264Parser.h
)

add_dependencies(avc2json cajun)

target_link_libraries(avc2json PRIVATE h264p)

# specify the C++ standard
target_compile_features(
    h264p
    PUBLIC cxx_std_14
)

# Test cases
enable_testing()

set(args ${PROJECT_SOURCE_DIR}/sample/svt_testset_420_720p50_klved_4774.h264)

add_test(NAME Parse
    COMMAND avc2json ${args})
