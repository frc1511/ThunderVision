project(ThunderVisionDesktopClient)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INC_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ThunderVisionClient)
target_link_libraries(${PROJECT_NAME} PRIVATE wpilib)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
