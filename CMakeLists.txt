cmake_minimum_required(VERSION 3.1)

project(RollingRaspberry LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wno-psabi)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CONFIG_JSON_DIR "/boot/frc1511" CACHE STRING "Config JSON Path")
option(BUILD_TOOLS "Build assorted tools" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(THUNDER_DEBUG ON)
  list(APPEND THUNDER_DEF_LIST "THUNDER_DEBUG=1")
endif()

set(THUNDER_SRC_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/src/lib")
set(THUNDER_INC_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
set(THUNDER_THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
set(THUNDER_TOOLS_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/tools")

# if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
#   message(FATAL_ERROR "Must be built on a Raspberry Pi!")
# endif()

# --- Sources ---

function(add_thunder_sources)
  set(THUNDER_SRC ${THUNDER_SRC} ${ARGV} PARENT_SCOPE)
endfunction()

include("${THUNDER_SRC_DIR}/CMakeLists.txt")

# --- Thirdparty ---

set(WPILIB_MODULES "wpiutil;wpimath;wpinet;ntcore;cscore;cameraserver;apriltag")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${THUNDER_THIRDPARTY_DIR}/allwpilib/cmake/modules")
set(WPILIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(WITH_JAVA OFF CACHE BOOL "" FORCE)
set(WITH_TESTS OFF CACHE BOOL "" FORCE)

foreach(MODULE ${WPILIB_MODULES})
  add_subdirectory("${THUNDER_THIRDPARTY_DIR}/allwpilib/${MODULE}" EXCLUDE_FROM_ALL)
endforeach()

# --- Build Service ---

add_executable(${PROJECT_NAME} ${THUNDER_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${THUNDER_INC_DIR})

# --- WPILib ---

foreach(MODULE ${WPILIB_MODULES})
  target_link_libraries(${PROJECT_NAME} PUBLIC ${MODULE})
endforeach()

# Fix WPILib linking on macOS
if(APPLE)
  target_link_libraries(${PROJECT_NAME} PUBLIC "-framework Foundation -framework AVFoundation -framework CoreMedia -framework CoreVideo")
endif()

# --- Pthread ---

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if(NOT Threads_FOUND)
  message(FATAL_ERROR "Pthread not found.")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# --- Definitions ---

target_compile_definitions(${PROJECT_NAME} PUBLIC ${THUNDER_DEF_LIST})

# --- Configuration File ---

configure_file(
  "${THUNDER_INC_DIR}/RollingRaspberry/config/config.h.in"
  "${THUNDER_INC_DIR}/RollingRaspberry/config/config.h"
)

# --- Build Tools ---

add_subdirectory(${THUNDER_TOOLS_DIR})

# --- Install ---

install(TARGETS ${PROJECT_NAME})
