cmake_minimum_required(VERSION 3.1)

project(RollingRaspberry LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(THUNDER_DEBUG ON)
  list(APPEND THUNDER_DEF_LIST "THUNDER_DEBUG=1")
endif()

set(THUNDER_SRC_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(THUNDER_INC_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(THUNDER_THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "Must be built on a Raspberry Pi!")
endif()

# --- Sources ---

function(add_thunder_sources)
  set(THUNDER_SRC ${THUNDER_SRC} ${ARGV} PARENT_SCOPE)
endfunction()

include("${THUNDER_SRC_DIR}/CMakeLists.txt")

# --- Thirdparty ---

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${THUNDER_THIRDPARTY_DIR}/allwpilib/cmake/modules")
set(WPILIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(WITH_JAVA OFF CACHE BOOL "" FORCE)
set(WITH_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory("${THUNDER_THIRDPARTY_DIR}/allwpilib/wpinet" EXCLUDE_FROM_ALL)
add_subdirectory("${THUNDER_THIRDPARTY_DIR}/allwpilib/wpiutil" EXCLUDE_FROM_ALL)
add_subdirectory("${THUNDER_THIRDPARTY_DIR}/allwpilib/ntcore" EXCLUDE_FROM_ALL)
add_subdirectory("${THUNDER_THIRDPARTY_DIR}/allwpilib/cscore" EXCLUDE_FROM_ALL)
add_subdirectory("${THUNDER_THIRDPARTY_DIR}/allwpilib/cameraserver" EXCLUDE_FROM_ALL)

# --- Build ---

add_executable(${PROJECT_NAME} ${THUNDER_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${THUNDER_INC_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC ntcore)
target_link_libraries(${PROJECT_NAME} PUBLIC cscore)
target_link_libraries(${PROJECT_NAME} PUBLIC cameraserver)

target_compile_definitions(${PROJECT_NAME} PUBLIC ${THUNDER_DEF_LIST})

# --- Install ---

install(TARGETS ${PROJECT_NAME})
