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

#include "src/imageStruct.hpp"
#include "src/renderImage.h"
#include "src/renderRim.h"
#include "src/triangleMesh.h"

#include "src/Rim.h"
#include "src/MainClass.hpp"

//#include <AntTweakBar.h>
#include <gtx/string_cast.hpp>

//#include <Ui/mkUiConfig.h>
//#include <Ui/mkUiTypes.h>

//#include <Ui/mkUi.h>

//#include <Ui/Nano/mkGlWindow.h>


//#ifndef KIUI_EXAMPLE_RESSOURCE_PATH
//    #define KIUI_EXAMPLE_RESSOURCE_PATH "data/"
//#endif

/*void WindowSizeCBMain(GLFWwindow *_window, int width, int height) {
    glViewport(0, 0, width, height);
    //std::cout << "Callback?" << std::endl;

    MainClass::width = width;
    MainClass::height = height;
    //MainClass::mvpreload = true;

    // Send the new window size to AntTweakBar
    //TwWindowSize(width, height);
}*/


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

    try {
        nanogui::init();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //float lineWidth[2];
        //glGetFloatv(GL_LINE_WIDTH_RANGE, lineWidth);

        MainClass *mainClass = new MainClass(filename, alpha_limit, errorMarginDegree);
        //mainClass.initTW();

        mainClass->drawAll();
        mainClass->setVisible(true);

        nanogui::mainloop();

        delete mainClass;
        nanogui::shutdown();
    } catch (const std::runtime_error &e) {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
#if defined(WIN32)
        MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
        std::cerr << error_msg << endl;
#endif
        return -1;
    }


    return 0;
}