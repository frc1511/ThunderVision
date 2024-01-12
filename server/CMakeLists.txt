project(ThunderVisionServer)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

include("${SRC_DIR}/CMakeLists.txt")

add_executable(${PROJECT_NAME} ${THUNDER_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${INC_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE wpilib)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

install(TARGETS ${PROJECT_NAME})
