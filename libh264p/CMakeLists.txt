cmake_minimum_required (VERSION 3.8)
project(libh264p)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (WIN32)
    set(CMAKE_INSTALL_LIBDIR $ENV{APPDATA})
endif()

aux_source_directory(. SRC_LIST)
add_library(libh264p STATIC ${SRC_LIST})
set_property(TARGET libh264p PROPERTY POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    target_link_libraries(libh264p)
else()
    set_target_properties(libh264p 
        PROPERTIES
        OUTPUT_NAME h264p)
endif()

include_directories(../../loki-lib/include)

target_include_directories(libh264p
          PUBLIC 
          "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
          "$<INSTALL_INTERFACE:include>"
          )

file(GLOB LIB_INCLUDE_FILES "./*.h")

# install the target and create export-set
install(TARGETS libh264p 
    EXPORT libh264pTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# install header file
install(FILES ${LIB_INCLUDE_FILES} DESTINATION include)

# generate and install export file
install(EXPORT libh264pTargets
        FILE libh264pTargets.cmake
        NAMESPACE lcss::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/lib/cmake/libh264p
)