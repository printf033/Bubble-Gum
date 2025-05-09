cmake_minimum_required(VERSION 3.16)

project(BubbleGum)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20 ")# -fsanitize=address

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

include_directories(${PROJECT_SOURCE_DIR}/mylog/inc)
include_directories(${PROJECT_SOURCE_DIR}/glad/include)
include_directories(${PROJECT_SOURCE_DIR}/opengl)
include_directories(${PROJECT_SOURCE_DIR}/net/inc)
include_directories(${PROJECT_SOURCE_DIR}/thr)

aux_source_directory(${PROJECT_SOURCE_DIR}/mylog/src LOG_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/net/src NET_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/glad/src GLAD_SRC)

aux_source_directory(${PROJECT_SOURCE_DIR}/opengl GL_MAIN)
aux_source_directory(${PROJECT_SOURCE_DIR}/net NET_MAIN)
aux_source_directory(${PROJECT_SOURCE_DIR}/bbg BBG_MAIN)
add_executable(opengl ${GL_MAIN} ${LOG_SRC} ${GLAD_SRC})
add_executable(net ${NET_MAIN} ${LOG_SRC} ${NET_SRC})
add_executable(bbg ${BBG_MAIN} ${LOG_SRC} ${GLAD_SRC} ${NET_SRC})

find_package(OpenGL REQUIRED)
target_link_libraries(opengl glfw OpenGL::GL assimp stb)
target_link_libraries(net event jsoncpp)
target_link_libraries(bbg glfw OpenGL::GL assimp stb event jsoncpp)