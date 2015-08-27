//
// Created by jerry on 24.08.15.
//

#ifndef ALPHATOMESH_MAINCLASS_HPP
#define ALPHATOMESH_MAINCLASS_HPP


#include <memory>
//#include <AntTweakBar.h>
#include "imageStruct.hpp"
#include "renderImage.h"
#include "renderRim.h"
#include "triangleMesh.h"
#include "Rim.h"

#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#if defined(WIN32)
#include <windows.h>
#endif
#include <nanogui/glutil.h>
#include <iostream>

//#include "../glfw/include/GLFW/glfw3.h"

class MainClass : public nanogui::Screen {
private:
    std::unique_ptr<imageStruct> img;
    std::unique_ptr<renderImage> renderImage1;

   // TwBar *animationBar;
   // TwBar *allgoBar;

    vector<renderRim> rimsToRender;
    vector<triangleMesh> meshToRender;

    string filename;
    string status;

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

    nanogui::GLShader mShader;
    nanogui::ProgressBar *mProgress;

public:
    MainClass(const string &filename, float alpha_limit = 0.9f, float errorMarginDegree = 1.f);

    ~MainClass();

    void mainLoop(float dt);

    void WindowSize();

    void initTW();
    void Save();
    virtual void draw(NVGcontext *ctx) override;
    virtual void drawContents()  override;

private:

    void init(Rim r);


    /*void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && mods != GLFW_MOD_CONTROL)
            stepp = true;

        if (key == GLFW_KEY_SPACE && mods == GLFW_MOD_CONTROL)
            stepp = true;
    }*/

    //friend void TW_CALL doStep(void *data);

    //friend void TW_CALL doAutoStep(void *data);

    //friend void TW_CALL initRims(void *data);

    //friend void TW_CALL loadImage(void *data);
    //friend void TW_CALL doSave(void *data);

    //friend void WindowSizeCB(GLFWwindow *_window, int width, int height);
    //friend void WindowSizeCBMain(GLFWwindow *_window, int width, int height);
public:
};

#endif //ALPHATOMESH_MAINCLASS_HPP
