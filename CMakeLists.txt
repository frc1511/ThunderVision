cmake_minimum_required(VERSION 3.1)

project(RollingRaspberry LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CONFIG_JSON_PATH "/boot/RollingRaspberryConfig.json" CACHE STRING "Config JSON Path")
option(PREBUILT_WPILIB "Download pre-built WPILib binaries instead of building them" OFF)
set(PREBUILT_WPILIB_VERSION "2023.1.1-beta-4" CACHE STRING "Version of WPILib pre-built binaries to download")

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

set(WPILIB_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/wpilib_${PREBUILT_WPILIB_VERSION}")

set(WPILIB_MODULES "wpiutil;wpimath;wpinet;ntcore;cscore;cameraserver")

if(PREBUILT_WPILIB)
  function(download_lib LIB_NAME LIB_URL LIB_DEST)
    message(STATUS "Downloading ${LIB_URL}")
    file(DOWNLOAD ${LIB_URL} ${LIB_DEST})
    message(STATUS "Downloaded ${LIB_NAME}")
  endfunction()
  
  function(extract_lib LIB_NAME ZIP_DIR LIB_DEST)
    message(STATUS "Extracting ${ZIP_DIR}")
    file(ARCHIVE_EXTRACT INPUT "${ZIP_DIR}" DESTINATION "${LIB_DEST}")
    message(STATUS "Extracted ${LIB_NAME}")
  endfunction()
  
  function(download_extract_lib LIB_NAME)
    set(LIB_URL "https://frcmaven.wpi.edu/artifactory/release/edu/wpi/first/${LIB_NAME}/${LIB_NAME}-cpp/${PREBUILT_WPILIB_VERSION}/${LIB_NAME}-cpp-${PREBUILT_WPILIB_VERSION}-linuxarm32static.zip")
    set(LIB_DEST "${WPILIB_DOWNLOAD_DIR}/${LIB_NAME}")
    
    download_lib(${LIB_NAME} ${LIB_URL} "${LIB_DEST}.zip")
    extract_lib(${LIB_NAME} "${LIB_DEST}.zip" ${LIB_DEST})
  endfunction()
  
  function(download_extract_headers LIB_NAME)
    set(INC_URL "https://frcmaven.wpi.edu/artifactory/release/edu/wpi/first/${LIB_NAME}/${LIB_NAME}-cpp/${PREBUILT_WPILIB_VERSION}/${LIB_NAME}-cpp-${PREBUILT_WPILIB_VERSION}-headers.zip")
    
    set(INC_DEST "${WPILIB_DOWNLOAD_DIR}/${LIB_NAME}_include")
    
    download_lib(${LIB_NAME} ${INC_URL} "${INC_DEST}.zip")
    extract_lib(${LIB_NAME} "${INC_DEST}.zip" ${INC_DEST})
  endfunction()
  
  foreach(MODULE ${WPILIB_MODULES})
    if (NOT EXISTS "${WPILIB_DOWNLOAD_DIR}/${MODULE}/linux/arm32/static/lib${MODULE}.a")
      download_extract_lib(${MODULE})
    endif()
    
    if (NOT EXISTS "${WPILIB_DOWNLOAD_DIR}/${MODULE}_include")
      download_extract_headers(${MODULE})
    endif()
  endforeach()
  
else()
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${THUNDER_THIRDPARTY_DIR}/allwpilib/cmake/modules")
  set(WPILIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
  
  set(WITH_JAVA OFF CACHE BOOL "" FORCE)
  set(WITH_TESTS OFF CACHE BOOL "" FORCE)
  
  foreach(MODULE ${WPILIB_MODULES})
    add_subdirectory("${THUNDER_THIRDPARTY_DIR}/allwpilib/${MODULE}" EXCLUDE_FROM_ALL)
  endforeach()
endif()

# --- AprilTag ---

add_subdirectory("${THUNDER_THIRDPARTY_DIR}/apriltag" EXCLUDE_FROM_ALL)

# --- Build ---

add_executable(${PROJECT_NAME} ${THUNDER_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${THUNDER_INC_DIR})

if(PREBUILT_WPILIB)
  target_link_directories(${PROJECT_NAME} PUBLIC ${WPILIB_DOWNLOAD_DIR})
  
  foreach(MODULE ${WPILIB_MODULES})
    target_link_libraries(${PROJECT_NAME} PUBLIC "${WPILIB_DOWNLOAD_DIR}/${MODULE}/linux/arm32/static/lib${MODULE}.a")
    target_include_directories(${PROJECT_NAME} PUBLIC "${WPILIB_DOWNLOAD_DIR}/${MODULE}_include")
  endforeach()
  
  # --- OpenCV ---
  
  find_package(OpenCV REQUIRED)
  if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found.")
  endif()
  
  target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
  target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
else()
  foreach(MODULE ${WPILIB_MODULES})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${MODULE})
  endforeach()
endif()

# --- Pthread ---

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if(NOT Threads_FOUND)
  message(FATAL_ERROR "Pthread not found.")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# --- AprilTags ---

target_link_libraries(${PROJECT_NAME} PUBLIC apriltag)

# --- Definitions ---

target_compile_definitions(${PROJECT_NAME} PUBLIC ${THUNDER_DEF_LIST})

# --- Configuration File ---

configure_file(
  "${THUNDER_INC_DIR}/RollingRaspberry/config/config.h.in"
  "${THUNDER_INC_DIR}/RollingRaspberry/config/config.h"
)

# --- Install ---

install(TARGETS ${PROJECT_NAME})
