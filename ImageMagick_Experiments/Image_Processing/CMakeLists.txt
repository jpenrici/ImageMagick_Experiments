cmake_minimum_required(VERSION 3.25)

project(ImageMagick_Experiment LANGUAGES CXX)

set(PROGRAM_NAME Image_Processing)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
  src/common.cpp
  src/main.cpp
  src/tool.cpp
)

set(RESOURCE_FILES
  Resources/bonsai.png
  Resources/bonsai.jpg
)

add_executable(${PROGRAM_NAME} ${SOURCES})

find_package(ImageMagick REQUIRED COMPONENTS Magick++)
if (ImageMagick_FOUND)
    include_directories(${ImageMagick_INCLUDE_DIRS})
    target_link_libraries(${PROGRAM_NAME} PRIVATE ${ImageMagick_LIBRARIES})
    message(STATUS "ImageMagick found:")
    message(STATUS "  Directory: ${ImageMagick_INCLUDE_DIRS}")
    message(STATUS "  Libraries: ${ImageMagick_LIBRARIES}")
else()
    message(FATAL_ERROR "PkgConfig or 'ImageMagick' not found. Please ensure the PkgConfig and 'ImageMagick' is installed on your system.")
endif()

foreach(FILE ${RESOURCE_FILES})
  file(COPY ${FILE} DESTINATION ${CMAKE_BINARY_DIR}/Resources)
endforeach()

install(TARGETS ${PROGRAM_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})
