# h264p
H.264/AVC Parser library and test application.

## To Build and Install
To build the library and test application do the the following:

1.  Clone `Loki` library, https://github.com/snaewe/loki-lib in the same directory as this project was cloned.

2.  Clone `CAJUN` JSON library, https://github.com/cajun-jsonapi/cajun-jsonapi in the same directory as this project was cloned.

3.  In h264p root directory, build and install this project using CMake:

	a. cmake -S . -B ./build
	
	b. cmake --build ./build
	
	c. cmake --install ./build
	
Add additional CMake parameters as required depending on your host development envirnoment.

The `--install` command will install a CMake package so it can be imported into other CMake projects.

## Run Test
To run the projects build-in test, enter the following:

	ctest --test-dir ./build

