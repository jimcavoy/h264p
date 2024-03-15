cmake_minimum_required (VERSION 3.21)
project(h264p
    VERSION 1.0.0
    DESCRIPTION "H.264 parser library"
    LANGUAGES C CXX)
	
include(GNUInstallDirs)

# This is the default install directory for config-file package cmake files
# It is relative to install prefix.
set(h264p_INSTALL_CMAKEDIR cmake CACHE PATH "Installation directory for config-file package cmake files")

# set the postfix "d" for the resulting .so or .dll files when building the
# library in debug mode
set(CMAKE_DEBUG_POSTFIX d )

# include the module `ExternalProject`
include(ExternalProject)

# Add an external project from a downloaded source archive
ExternalProject_Add(
  loki-lib
  URL https://github.com/snaewe/loki-lib/archive/refs/tags/Release_0_1_5.tar.gz
  URL_HASH MD5=74e60c683f745dc15c6e772927349483
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(loki-lib SOURCE_DIR)

set(LOKI_DIR "${SOURCE_DIR}")

include_directories(${LOKI_DIR}/include)

add_library(h264p STATIC)

target_sources(h264p PRIVATE
    include/h264p/h264prsr.h
    include/h264p/nalu.h
    include/h264p/naluimpl.h
    include/h264p/pps.h
    include/h264p/sei.h
    include/h264p/seiimpl.h
    include/h264p/slice.h
    include/h264p/sps.h
    src/bitstream.cpp
    src/bitstream.h
    src/h264prsr.cpp
    src/nalu.cpp
    src/naluimpl.cpp
    src/pps.cpp
    src/sei.cpp
    src/seiimpl.cpp
    src/slice.cpp
    src/sps.cpp
    src/util.cpp
    src/util.h
    src/vlc.cpp
    src/vlc.h
)

set_property(TARGET h264p PROPERTY POSITION_INDEPENDENT_CODE ON)

add_dependencies(h264p loki-lib)

# specify the C++ standard
target_compile_features(h264p
    PUBLIC cxx_std_14
)

target_include_directories(h264p
     PRIVATE src/h264p
     PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	 INTERFACE $<BUILD_INTERFACE:${LOKI_DIR}/include>
)

install(TARGETS h264p
    EXPORT h264p_export
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install (
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install (EXPORT h264p_export
    FILE h264p-config.cmake
    NAMESPACE thetastream::
    DESTINATION ${h264p_INSTALL_CMAKEDIR}
)

# Defines write_basic_package_version_file
include(CMakePackageConfigHelpers)

# Create a package version file for the package. This will allow consumers to specify
# version argument to select between several versions of the package via specifying VERSION argument
# to find_package().
# We're not explicitly specifying the VERSION parameter to allow `write_basic_package_version_file` to 
# get it from project(...) version (which is 1.0.0)
write_basic_package_version_file(
  "h264p-config-version.cmake"
  # Package compatibility strategy. SameMajorVersion is essentially `semantic versioning`.
  COMPATIBILITY SameMajorVersion
)

# Install command for deploying Config-file package files into the target system. It must be present
# in the same directory as `h264p-config.cmake` file.
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/h264p-config-version.cmake"
  DESTINATION "${h264p_INSTALL_CMAKEDIR}"
)
