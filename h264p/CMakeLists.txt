cmake_minimum_required (VERSION 3.21)
project(h264p
    VERSION 0.2.0 
    DESCRIPTION "H.264 parser library"
    LANGUAGES C CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(../../loki-lib/include)

add_library(h264p STATIC)

file(GLOB SRC_LIST src/*.cpp)

target_sources(h264p PRIVATE ${SRC_LIST})

set_property(TARGET h264p PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(h264p
          PRIVATE src/h264p
		  PUBLIC include
          )

include(GNUInstallDirs)

install(TARGETS h264p)

install (
	DIRECTORY include/
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
