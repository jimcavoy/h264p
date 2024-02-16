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

include_directories(../../loki-lib/include)

add_library(h264p STATIC)

file(GLOB SRC_LIST src/*.cpp)

target_sources(h264p PRIVATE ${SRC_LIST})

set_property(TARGET h264p PROPERTY POSITION_INDEPENDENT_CODE ON)

# specify the C++ standard
target_compile_features(
    h264p
    PUBLIC cxx_std_14
)

target_include_directories(h264p
          PRIVATE src/h264p
		  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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
        NAMESPACE lcss::
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
  "mp2tp-config-version.cmake"
  # Package compatibility strategy. SameMajorVersion is essentially `semantic versioning`.
  COMPATIBILITY SameMajorVersion
)

# Install command for deploying Config-file package files into the target system. It must be present
# in the same directory as `h264p-config.cmake` file.
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/mp2tp-config-version.cmake"
  DESTINATION "${h264p_INSTALL_CMAKEDIR}"
)