# h264p
__h264p__ project provides a library to parse an H.264/AVC byte sequence at the Network Abstraction Layer (NAL).

The project can be build and run on both Windows and Linux systems.

## Project Structure
The __h264p__ project consists of a static library and an example application.

### Static Library (mp2tp)
The static library __h264p__ provides classes to parse an H.264/AVC byte sequence at the Network Abstraction Layer (NAL).

### Example Application (avc2json)
The example application, __avc2json__, reads an H.264 file and outputs JSON representation to console.


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

