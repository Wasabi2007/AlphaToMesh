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

#include <AntTweakBar.h>

imageStruct *img;
renderImage *renderImage1;

string filename;

float alpha_limit = 0.9f;
float errorMarginDegree = 1.f;

bool stepp = false;

void init(Rim &r, vector<renderRim> &rimsToRender, vector<triangleMesh> &meshToRender) {


    std::cout << std::fixed << std::setprecision(1);
    /*for (unsigned y = 0; y < img->height; ++y) {
        for (unsigned x = 0; x < img->width; ++x) {
            std::cout << (img->getPixel(x, y).a>0.5?"#":(img->getPixel(x, y).a>0.25?"+":(img->getPixel(x, y).a>0.01?"-":" ")));
        }
        std::cout << std::endl;
    }*/

    auto rims = r.getSimpelRims();

    for (const auto &rim : rims) {
        rimsToRender.emplace_back(rim, long(img->width), long(img->height));
    }
    for (const auto &rim: rims) {
        /*cout << endl << "simple rim" << endl;
        for(auto& v : rim){
            cout<< "("<< v.x << "/"<< v.y << ")->";
        }*/
        //rimsToRender.emplace_back(rim,long(img->width),long(img->height));
        meshToRender.emplace_back(rim, long(img->width), long(img->height));
    }

    //glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void mainLoop(float dt, vector<renderRim> &rimsToRender, vector<triangleMesh> &meshToRender) {
    glClearColor(0.4f, 0.6f, 0.9f, 1.f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glLineWidth(5.0f);
    //glPointSize(50.f);
    renderImage1->Render();

    for (auto &r : meshToRender) {
        r.Render();
    }

    for (auto &r : rimsToRender) {
        r.Render();
    }

    if (stepp) {
        stepp = false;
        for (auto &r : meshToRender) {
            r.Step();
        }
    }


}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && mods != GLFW_MOD_CONTROL)
        stepp = true;

    if (key == GLFW_KEY_SPACE && mods == GLFW_MOD_CONTROL)
        stepp = true;
}

void TW_CALL doStep(void* data){
    stepp = true;
}


int main(int argc, char *argv[]) {

    Util::initStacktrace(argv[0]);

    vector<renderRim> rimsToRender;
    vector<triangleMesh> meshToRender;
    //std::ifstream is;
    //is.open("test.png",std::ifstream::in | ios_base::binary);
    //std::cout << "png is valid? " << validate(is) << std::endl;
    //is.close();

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

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    const auto width = 800 * 2;
    const auto height = 600 * 2;
    GLFWwindow *window = glfwCreateWindow(width, height, "AlphaToMesh", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    glewExperimental = GL_TRUE;
    glewInit();


    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(width, height);
    TwBar *myBar;
    myBar = TwNewBar("NameOfMyTweakBar");
    TwAddButton(myBar,"Step",doStep,NULL," label='Step' ");


    // after GLFW initialization
    // directly redirect GLFW events to AntTweakBar
    glfwSetMouseButtonCallback(window,(GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
    glfwSetCursorPosCallback(window,(GLFWcursorposfun)TwEventCursorPosGLFW3);
    glfwSetScrollCallback(window,(GLFWscrollfun)TwEventScrollGLFW3);
    glfwSetKeyCallback(window,(GLFWkeyfun)TwEventKeyGLFW3);
    glfwSetCharCallback(window,(GLFWcharfun)TwEventCharModsGLFW3);
    cout << TwGetLastError() << endl;

    // send window size events to AntTweakBar
    //glfwSetWindowSizeCallback(MyResize); // and call TwWindowSize in the function MyResize


    img = imageStruct::load(filename.c_str());
    renderImage1 = new renderImage(*img);
    Rim r{alpha_limit, img, errorMarginDegree};
    init(r, rimsToRender, meshToRender);

    float lineWidth[2];
    glGetFloatv(GL_LINE_WIDTH_RANGE, lineWidth);

    //cout << lineWidth[0] << " " << lineWidth[1] << endl;

    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();

        static auto time = std::chrono::high_resolution_clock::now();
        static float dt = 0.f;
        dt = std::chrono::duration_cast<std::chrono::duration<float>>(
                time - std::chrono::high_resolution_clock::now()).count();
        mainLoop(dt, rimsToRender, meshToRender);
        TwDraw();
        time = std::chrono::high_resolution_clock::now();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
    TwTerminate();
    glfwTerminate();

    return 0;
}