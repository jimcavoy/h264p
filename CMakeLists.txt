# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)
project (h264p
	VERSION 1.0.1
	DESCRIPTION "Advanced Video Coding (AVC)/H.264 Parser library and example application, avc2json"
	LANGUAGES CXX)

# Include sub-projects.
add_subdirectory (h264p)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS json)

add_executable(avc2json)

target_sources(avc2json
  PRIVATE
    src/main.cpp
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

option(ENABLE_DOCUMENTATION "Enable documentation" ON)

if (ENABLE_DOCUMENTATION)
############################################################################
# Doxygen configuration
############################################################################
# Locate Doxygen in the environment. This will set DOXYGEN_FOUND variable 
# as a result. When Doxygen is found, DOXYGEN_VERSION variable will be 
# also set to found Doxygen version.
find_package(Doxygen REQUIRED)

# Override the default doxygen output directory, which is CMAKE_CURRENT_BINARY_DIR. 
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs")
# Enable HTML documentation generation
set(DOXYGEN_GENERATE_HTML YES)
# Enable MAN page generation
set(DOXYGEN_GENERATE_MAN NO)
# Enable markdown support
set(DOXYGEN_MARKDOWN_SUPPORT YES)
# Enable autolink support
set(DOXYGEN_AUTOLINK_SUPPORT YES)
# Enable dot graph support
set(DOXYGEN_HAVE_DOT YES)
# Include collaboration diagrams in class documentation
set(DOXYGEN_COLLABORATION_GRAPH NO)
# Include class diagrams in class documentation
set(DOXYGEN_CLASS_GRAPH YES)
# Generate UML-like graphs
set(DOXYGEN_UML_LOOK YES)
# Include type and parameter information in UML graphs
set(DOXYGEN_DOT_UML_DETAILS YES)
# Max per-line length for graph content
set(DOXYGEN_DOT_WRAP_THRESHOLD 100)
# Enables drawing call graphs for the functions
set(DOXYGEN_CALL_GRAPH YES)
# Suppress the output messages
set(DOXYGEN_QUIET YES)
############################################################################

doxygen_add_docs(
  h264p_docs
  Index.md h264p/ src/
  ALL
  COMMENT "Generating documentation for h264p library"
)
endif()


# Test cases
enable_testing()

add_test(NAME Parse
  COMMAND avc2json ${PROJECT_SOURCE_DIR}/sample/svt_testset_420_720p50_klved_4774.h264
)
