# h264p
H.264/AVC Parser library and test application.

## To Build and Install
To build the library and test application do the the following:

In h264p root directory, build and install this project using CMake:

1. cmake -S . -B ./build
	
2. cmake --build ./build
	
3. cmake --install ./build
	
Add additional CMake parameters as required depending on your host development envirnoment.

The `--install` command will install a CMake package so it can be imported into other CMake projects.

## Run Test
To run the projects build-in test, enter the following:

	ctest --test-dir ./build

