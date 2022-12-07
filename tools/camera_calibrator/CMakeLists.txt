cmake_minimum_required(VERSION 3.1)

project(ThunderCameraCalibrator LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(THUNDER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(THUNDER_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(THUNDER_SRC "")
function(add_thunder_sources)
  set(THUNDER_SRC ${THUNDER_SRC} ${ARGV} PARENT_SCOPE)
endfunction()

include("${THUNDER_SRC_DIR}/CMakeLists.txt")

# --- Build ---

add_executable(${PROJECT_NAME} ${THUNDER_SRC})

# --- Thirdparty ---

find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
  message(FATAL_ERROR "OpenCV not found.")
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})

# --- Definitions ---

target_compile_definitions(${PROJECT_NAME} PUBLIC ${THUNDER_DEF_LIST})