cmake_minimum_required(VERSION 3.16)

project(BubbleGum)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20 ")#-fsanitize=address

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

include_directories(${PROJECT_SOURCE_DIR}/glad/include)
aux_source_directory(${PROJECT_SOURCE_DIR}/glad/src GLAD_SRC)
include_directories(${PROJECT_SOURCE_DIR}/mylog/inc)
aux_source_directory(${PROJECT_SOURCE_DIR}/mylog/src LOG_SRC)
include_directories(${PROJECT_SOURCE_DIR}/opengl)
include_directories(${PROJECT_SOURCE_DIR}/control)
include_directories(${PROJECT_SOURCE_DIR}/mynet)
include_directories(${PROJECT_SOURCE_DIR}/mythr)
include_directories(${PROJECT_SOURCE_DIR}/physics)

add_executable(bbg ${PROJECT_SOURCE_DIR}/bbg/main.cpp ${LOG_SRC} ${GLAD_SRC})
find_package(OpenGL REQUIRED)
target_link_libraries(bbg glfw OpenGL::GL assimp stb jsoncpp)
