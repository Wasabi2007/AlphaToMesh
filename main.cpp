#define GLM_FORCE_RADIANS

#include <iostream>
#include <fstream>      // std::filebuf
#include <vector>
#include <iomanip>
#include <chrono>
#include <algorithm>

//#define PNG_DEBUG 3
//#include <lodepng/lodepng.h>
#include "glm/glm.hpp"
#include "glm/gtx/vector_angle.hpp"
//#include "lodepng/lodepng.h"

#include "glew/include/GL/glew.h"
#include "glfw/include/GLFW/glfw3.h"

#include <stdexcept>

#include "util/stacktrace.hpp"

#include "src/imageStruct.hpp"
#include "src/renderImage.h"
#include "src/renderRim.h"
#include "src/triangleMesh.h"

#include "src/Rim.h"
#include "src/MainClass.hpp"

//#include <AntTweakBar.h>
#include <gtx/string_cast.hpp>

void WindowSizeCBMain(GLFWwindow *_window, int width, int height) {
    glViewport(0, 0, width, height);
    //std::cout << "Callback?" << std::endl;

    MainClass::width = width;
    MainClass::height = height;
    //MainClass::mvpreload = true;

    // Send the new window size to AntTweakBar
    //TwWindowSize(width, height);
}


int main(int argc, char *argv[]) {

    Util::initStacktrace(argv[0]);
    //std::ifstream is;
    //is.open("test.png",std::ifstream::in | ios_base::binary);
    //std::cout << "png is valid? " << validate(is) << std::endl;
    //is.close();
    string filename;
    float alpha_limit = 0.9f;
    float errorMarginDegree = 1.0f;

    if (argc > 1) {
        filename = argv[1];
    } else {
        filename = "test0.png";
    }

    if (argc > 2) {
        alpha_limit = stof(argv[2]);
    }
    if (argc > 3) {
        errorMarginDegree = stof(argv[3]);
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    const auto width = 800*2;
    const auto height = 600*2;
    GLFWwindow *window = glfwCreateWindow(width, height, "AlphaToMesh", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
    glfwMakeContextCurrent(window);

    //glfwSetKeyCallback(window, key_callback);

    glewExperimental = GL_TRUE;
    glewInit();


    // after GLFW initialization
    // directly redirect GLFW events to AntTweakBar
    //glfwSetMouseButtonCallback(window,(GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
    //glfwSetCursorPosCallback(window,(GLFWcursorposfun)TwEventCursorPosGLFW3);
    //glfwSetScrollCallback(window,(GLFWscrollfun)TwEventScrollGLFW3);
    //glfwSetKeyCallback(window,(GLFWkeyfun)TwEventKeyGLFW3);
    //glfwSetCharCallback(window,(GLFWcharfun)TwEventCharModsGLFW3);

    //glfwSetWindowSizeCallback(window,WindowSizeCB);
    //cout << TwGetLastError() << endl;

    // send window size events to AntTweakBar
    //glfwSetWindowSizeCallback(MyResize); // and call TwWindowSize in the function MyResize




    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float lineWidth[2];
    glGetFloatv(GL_LINE_WIDTH_RANGE, lineWidth);

    MainClass mainClass{filename,alpha_limit,errorMarginDegree};
    mainClass.initTW();

    WindowSizeCBMain(nullptr, width, height);

    //cout << lineWidth[0] << " " << lineWidth[1] << endl;

    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();

        static auto time = std::chrono::high_resolution_clock::now();
        static float dt = 0.f;

        auto t2 = std::chrono::high_resolution_clock::now();
        dt = float(std::chrono::duration_cast<std::chrono::milliseconds>(t2-time).count())/1000;
        mainClass.mainLoop(dt);
        //TwDraw();
        time = t2;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
    //TwTerminate();
    glfwTerminate();

    return 0;
}