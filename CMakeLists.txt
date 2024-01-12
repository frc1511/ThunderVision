cmake_minimum_required(VERSION 3.5)

project(ThunderVision LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wno-psabi)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

option(BUILD_TOOLS "Build assorted tools" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(THUNDER_DEBUG ON)
endif()

set(CLIENT_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/client")
set(SERVER_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/server")
set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
set(TOOLS_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/tools")

function(add_thunder_sources)
  set(THUNDER_SRC ${THUNDER_SRC} ${ARGV} PARENT_SCOPE)
endfunction()

#
# WPIlib
#
set(WPILIB_MODULES "wpiutil;wpimath;wpinet;ntcore;cscore;cameraserver;apriltag")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${THIRDPARTY_DIR}/allwpilib/cmake/modules")
set(WPILIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(WITH_JAVA OFF CACHE BOOL "" FORCE)
set(WITH_TESTS OFF CACHE BOOL "" FORCE)

foreach(MODULE ${WPILIB_MODULES})
  add_subdirectory("${THIRDPARTY_DIR}/allwpilib/${MODULE}" EXCLUDE_FROM_ALL)
endforeach()

add_library(wpilib INTERFACE)
target_link_libraries(wpilib INTERFACE ${WPILIB_MODULES})

# Fix WPILib linking on macOS
if(APPLE)
  target_link_libraries(wpilib INTERFACE "-framework Foundation"
                                         "-framework AVFoundation"
                                         "-framework CoreMedia"
                                         "-framework CoreVideo")
endif()

#
# pthread
#
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if(NOT Threads_FOUND)
  message(FATAL_ERROR "Pthread not found.")
endif()

#
# Build client.
#
add_subdirectory(${CLIENT_DIR})

#
# Build server.
#
add_subdirectory(${SERVER_DIR})

#
# Build tools.
#
if(BUILD_TOOLS)
  add_subdirectory(${TOOLS_DIR})
endif()
