cmake_minimum_required(VERSION 2.8)
project(AntTweakBar)

include(FindOpenGL)
#include(FindSDL)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Project includes
set(ATB_INCLUDE_DIR include)

# Standard Packages
#find_package(OpenGL REQUIRED)
#find_package(GLUT)
#find_library(GLFW_LIBRARY glfw)
if(WIN32)
    #remove_definitions(-DWIN32)
 add_definitions(-D_DEBUG -D_WINDOWS -D_WIN32 -D__PLACEMENT_NEW_INLINE -DTW_STATIC)# -D_USRDLL -DTW_EXPORTS)
endif()
if(UNIX)
 add_definitions(-D_UNIX -D__PLACEMENT_NEW_INLINE)
endif()

include_directories( ${ATB_INCLUDE_DIR}
                     ${OPENGL_INCLUDE_DIR}
                     ${GLFW_INCLUDE_DIR})

set( SOURCES src/res/TwXCursors.h
             include/AntTweakBar.h
             src/AntPerfTimer.h
             src/resource.h
             src/MiniGLFW3.h
             src/TwGraph.h
             src/LoadOGL.cpp
             src/LoadOGLCore.cpp
             src/TwColors.cpp
             src/TwMgr.cpp
             src/TwPrecomp.cpp
             src/TwEventSFML.cpp
             src/TwOpenGL.cpp
             src/TwOpenGLCore.cpp
             src/TwBar.cpp
             src/TwFonts.cpp
             #src/TwEventGLUT.c
             src/TwEventGLFW.c
             src/TwEventGLFW3.c
             #src/TwEventSDL.c
             #src/TwEventSDL12.c
             #src/TwEventSDL13.c
             )


if(UNIX)
    set(SOURCES ${SOURCES} src/TwEventX11.c)
endif()

add_library(AntTweakBar STATIC ${SOURCES})

message("Extra libs: ${EXTRA_LIBS}")
target_link_libraries(AntTweakBar  ${GLUT_LIBRARY}
                                   ${OPENGL_LIBRARY}
                                   ${EXTRA_LIBS})


#install(TARGETS AntTweakBar LIBRARY DESTINATION lib)

#add_subdirectory(examples)