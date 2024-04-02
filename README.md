# h264p
H.264/AVC Parser library and test application, __avc2json__.

## Prerequisites
This project requires the following:

1. Install CMake, https://cmake.org/
2. Install vcpkg, https://github.com/microsoft/vcpkg, which is a packet management system.
3. Configure `VCPKG_ROOT` environment variable to the path where you installed __vcpkg__.

## To Build and Install
In __h264p__ root directory, build and install this project using CMake:

### 1. Generate the build environment
    cmake --preset=<windows-base|linux-base>

For Windows platform, use `window-base` and for Linux platform use `linux-base` preset.

### 2. Build the library and test application
    cmake --build ./build 
### 3. Install the H.264 Parser library
    cmake --install ./build
	
Add additional CMake parameters as required depending on your host development environment.

The `--install` command will install a CMake package so it can be imported into other CMake projects.

## Run Test
To run the projects build-in test, enter the following:

	ctest --test-dir ./build

