cmake_minimum_required(VERSION 3.1)

set(THUNDER_TOOLS "camera_calibrator")

foreach(TOOL ${THUNDER_TOOLS})
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${TOOL}")
endforeach()
