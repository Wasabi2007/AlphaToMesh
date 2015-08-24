//
// Created by jerry on 24.08.15.
//

#ifndef ALPHATOMESH_MAINCLASS_HPP
#define ALPHATOMESH_MAINCLASS_HPP


#include <memory>
#include <AntTweakBar.h>
#include "imageStruct.hpp"
#include "renderImage.h"
#include "renderRim.h"
#include "triangleMesh.h"
#include "Rim.h"
#include "../glfw/include/GLFW/glfw3.h"

class MainClass {
private:
    std::unique_ptr<imageStruct> img;
    std::unique_ptr<renderImage> renderImage1;

    TwBar *animationBar;
    TwBar *allgoBar;

    vector<renderRim> rimsToRender;
    vector<triangleMesh> meshToRender;

    string filename;

    static bool mvpreload;

    static int width;
    static int height;

    float alpha_limit;

    float errorMarginDegree;
    bool stepp;
    bool canStep;

    bool autoStep;
    float timeCount;

    float stepTime;
public:

    MainClass(const string &filename, float alpha_limit = 0.9f, float errorMarginDegree = 1.f);

    void mainLoop(float dt);

    void WindowSize();

    void initTW();
private:

    void init(Rim r);

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && mods != GLFW_MOD_CONTROL)
            stepp = true;

        if (key == GLFW_KEY_SPACE && mods == GLFW_MOD_CONTROL)
            stepp = true;
    }

    friend void TW_CALL doStep(void *data);

    friend void TW_CALL doAutoStep(void *data);

    friend void TW_CALL initRims(void *data);

    friend void TW_CALL loadImage(void *data);

    friend void WindowSizeCB(GLFWwindow *_window, int width, int height);
    friend void WindowSizeCBMain(GLFWwindow *_window, int width, int height);
};

#endif //ALPHATOMESH_MAINCLASS_HPP
