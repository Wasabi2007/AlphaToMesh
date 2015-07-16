cmake_minimum_required(VERSION 3.2)
project(AlphaToMesh)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#include_directories(Libs/MinGW/libpng/header)
link_directories(Libs/MinGW/dlls)


#file(COPY Libs/MinGW/dlls/ DESTINATION .)
file(COPY data/ DESTINATION .)

add_subdirectory(glm)
add_subdirectory(glew)
add_subdirectory(glfw)
add_subdirectory(lodepng)

set(SOURCE_FILES main.cpp)
add_executable(AlphaToMesh ${SOURCE_FILES})
target_link_libraries(AlphaToMesh glm glfw GLEW_static opengl32 lodepng)