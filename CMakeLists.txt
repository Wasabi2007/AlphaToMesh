cmake_minimum_required(VERSION 3.2)
project(AlphaToMesh)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -ansi -fno-strict-aliasing -std=c++14")

add_definitions("-DSTACKTRACE" "-DGLEW_STATIC")
#include_directories(Libs/MinGW/libpng/header)
link_directories(Libs/MinGW/dlls)


#file(COPY Libs/MinGW/dlls/ DESTINATION .)
file(COPY data/ DESTINATION .)

add_subdirectory(glm)
include_directories(glm)
add_subdirectory(glew)
add_subdirectory(glfw)
add_subdirectory(lodepng)
add_subdirectory(AntTweakBar)
include_directories(lodepng)
include_directories(AntTweakBar/include)



SET(GCC_COVERAGE_LINK_FLAGS    "-static-libgcc -static-libstdc++")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )

set(SOURCE_FILES main.cpp util/stacktrace.hpp util/stacktrace.cpp src/MainClass.cpp src/MainClass.hpp)
add_executable(AlphaToMesh ${SOURCE_FILES} src/imageStruct.hpp src/renderImage.cpp src/renderImage.h src/renderRim.cpp src/renderRim.h src/shader.h src/triangleMesh.cpp src/triangleMesh.h src/Rim.cpp src/Rim.h)
if(WIN32)
    set(libs imagehlp)
else(WIN32)
    set(libs X11 Xxf86vm Xrandr pthread Xi Xinerama Xcursor)
endif(WIN32)

set(libs AntTweakBar ${libs} glm glfw GLEW_static)


if(WIN32)
    set(libs ${libs} opengl32 )
else(WIN32)
    set(libs ${libs} GL )
endif(WIN32)


target_link_libraries(AlphaToMesh ${libs} lodepng)