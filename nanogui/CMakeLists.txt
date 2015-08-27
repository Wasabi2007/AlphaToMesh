cmake_minimum_required (VERSION 2.8.3)

project("NanoGUI")

# Run simple C converter to put font files into the data segment
add_executable(bin2c resources/bin2c.c)
set(bin2c_cmdline resources.cpp resources.h)
file(GLOB resources "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.ttf")
foreach(file ${resources})
  list(APPEND bin2c_cmdline ${file})
endforeach()

include(ExternalProject)

add_custom_command(
  OUTPUT resources.cpp resources.h
  COMMAND bin2c ARGS ${bin2c_cmdline}
  DEPENDS bin2c ${resources}
  COMMENT "Running bin2c"
  PRE_BUILD VERBATIM)

include_directories(${CMAKE_CURRENT_BINARY_DIR})


option(GLFW_BUILD_EXAMPLES " " OFF)
option(GLFW_BUILD_TESTS " " OFF)
option(GLFW_BUILD_DOCS " " OFF)
option(GLFW_INSTALL " " OFF)

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../glfw" "glfw")

set(extra_source "")

if(WIN32)
  list(APPEND extra_source "${CMAKE_CURRENT_SOURCE_DIR}/ext/glew/src/glew.c")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set(extra_libs opengl32)
elseif (APPLE)
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
  # Find standard libraries needed for OpenGL/GLFW
  find_library(cocoa_library Cocoa)
  find_library(opengl_library OpenGL)
  find_library(corevideo_library CoreVideo)
  find_library(iokit_library IOKit)
  set(extra_libs ${cocoa_library} ${opengl_library} ${corevideo_library} ${iokit_library})
  list(APPEND extra_source src/darwin.mm)
elseif("${CMAKE_SYSTEM}" MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set(extra_libs GL Xxf86vm Xrandr Xinerama Xcursor Xi X11 pthread dl)
endif()

include_directories(ext/eigen ${GLFW_INCLUDE_DIR} ext/nanovg/src include ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/ext/glew/include)
add_library(nanogui STATIC
  ext/nanovg/src/nanovg.c resources.cpp
  include/nanogui/formscreen.h src/formscreen.cpp
  include/nanogui/glutil.h src/glutil.cpp
  include/nanogui/nanogui.h src/nanogui.cpp
  include/nanogui/widget.h src/widget.cpp
  include/nanogui/theme.h src/theme.cpp
  include/nanogui/layout.h src/layout.cpp
  include/nanogui/screen.h src/screen.cpp
  include/nanogui/label.h src/label.cpp
  include/nanogui/window.h src/window.cpp
  include/nanogui/popup.h src/popup.cpp
  include/nanogui/checkbox.h src/checkbox.cpp
  include/nanogui/button.h src/button.cpp
  include/nanogui/toolbutton.h
  include/nanogui/popupbutton.h src/popupbutton.cpp
  include/nanogui/combobox.h src/combobox.cpp
  include/nanogui/progressbar.h src/progressbar.cpp
  include/nanogui/slider.h src/slider.cpp
  include/nanogui/messagedialog.h src/messagedialog.cpp
  include/nanogui/textbox.h src/textbox.cpp
  include/nanogui/imagepanel.h src/imagepanel.cpp
  include/nanogui/imageview.h src/imageview.cpp
  include/nanogui/vscrollpanel.h src/vscrollpanel.cpp
  include/nanogui/colorwheel.h src/colorwheel.cpp
  include/nanogui/opengl.h
  ${extra_source}
)

file(GLOB NANOVG_HEADERS ext/nanovg/src/*.h)

link_directories(${CMAKE_CURRENT_BINARY_DIR}/ext_build/src/glfw_p-build/src)
add_executable(example src/example.cpp)
target_link_libraries(example nanogui glfw ${extra_libs})
file(COPY resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# vim: set et ts=2 sw=2 ft=cmake nospell:
