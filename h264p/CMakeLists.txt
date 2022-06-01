# CMakeList.txt : CMake project for h264p, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(h264p)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source to this project's executable.
add_executable (h264p "h264p.cpp" "MDPMParser.cpp" "SEIParser.cpp" "TestH264Parser.cpp")

list(APPEND EXTRA_LIBS libh264p)

include_directories(../../loki-lib/include)
include_directories(../../cajun-jsonapi/include/cajun)

target_link_libraries(h264p PUBLIC ${EXTRA_LIBS})

# TODO: Add tests and install targets if needed.
install(TARGETS h264p 
    EXPORT h264pTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)