# CMakeList.txt : CMake project for h264p, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(h264p)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (WIN32)
    set(LOKI_DIRECTORY ../../loki-lib/include)
    set(CAJUN_DIRECTORY ../../cajun-jsonapi/include/cajun)
    add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)
else()
    set(LOKI_DIRECTORY $ENV{HOME}/.vs/loki-lib/include)
    set(CAJUN_DIRECTORY $ENV{HOME}/.vs/cajun-jsonapi/include/cajun)
endif()

# Add source to this project's executable.
file(GLOB SRC_LIST "./*.cpp")
file(GLOB LOKI_SRC_LIST ${LOKI_DIRECTORY}/loki/*.h)
file(GLOB CAJUN_SRC_LIST ${CUJUN_DIRECTORY}/json/*.h)

set(HEADER_FILES ${LOKI_SRC_LIST} ${CAJUN_SRC_LIST})

add_executable(h264p ${SRC_LIST} ${HEADER_FILES})

target_include_directories(h264p PRIVATE ${LOKI_DIRECTORY})
target_include_directories(h264p PRIVATE ${CAJUN_DIRECTORY})

list(APPEND EXTRA_LIBS libh264p)

target_link_libraries(h264p PUBLIC ${EXTRA_LIBS})

# TODO: Add tests and install targets if needed.
install(TARGETS h264p 
    EXPORT h264pTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)