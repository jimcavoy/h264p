# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
project ("h264p")

# Include sub-projects.
add_subdirectory ("h264p")
add_subdirectory ("libh264p")
