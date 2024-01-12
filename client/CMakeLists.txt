project(ThunderVisionClient)

add_library(${PROJECT_NAME} ThunderVisionClient.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC wpilib)
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
