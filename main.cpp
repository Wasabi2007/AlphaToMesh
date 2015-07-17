#include <iostream>
#include <fstream>      // std::filebuf
#include <vector>
#include <iomanip>
#include <chrono>
#include <algorithm>

//#define PNG_DEBUG 3
//#include <lodepng/lodepng.h>
#include "glm/glm.hpp"
#include "glm/gtx/perpendicular.hpp"
#include "lodepng/lodepng.h"

#include "glew/include/GL/glew.h"
#include "glfw/include/GLFW/glfw3.h"

#include <stdexcept>

#include "util/stacktrace.hpp"

#include "src/imageStruct.hpp"
#include "src/renderImage.h"

imageStruct* img;
renderImage* renderImage1;
void init(){
    img = imageStruct::load("test.png");
    renderImage1 = new renderImage(*img);

    std::cout << std::fixed << std::setprecision(1);
    /*for (unsigned y = 0; y < img->height; ++y) {
        for (unsigned x = 0; x < img->width; ++x) {
            std::cout << (img->getPixel(x, y).a>0.5?"#":(img->getPixel(x, y).a>0.25?"+":(img->getPixel(x, y).a>0.01?"-":" ")));
        }
        std::cout << std::endl;
    }*/

    auto alpha_limit = 0.99f;

    std::vector<std::vector<ivec2>> rims;
    std::vector<ivec2> rim;

    std::vector<std::vector<ivec2>> directions; //todo find a method to compute this
    directions.resize(8);

    directions.at(0).push_back(ivec2(1,0)); //walking directionm
    directions.at(0).push_back(ivec2(0,-1)); //where ist rigth ?

    directions.at(1).push_back(ivec2(1,1));
    directions.at(1).push_back(ivec2(0,-1));

    directions.at(2).push_back(ivec2(0,1));
    directions.at(2).push_back(ivec2(1,0));

    directions.at(3).push_back(ivec2(-1,1));
    directions.at(3).push_back(ivec2(1,0));

    directions.at(4).push_back(ivec2(-1,0));
    directions.at(4).push_back(ivec2(0,1));

    directions.at(5).push_back(ivec2(-1,-1));
    directions.at(5).push_back(ivec2(0,1));

    directions.at(6).push_back(ivec2(0,-1));
    directions.at(6).push_back(ivec2(-1,0));

    directions.at(7).push_back(ivec2(1,-1));
    directions.at(7).push_back(ivec2(-1,0));

    for (unsigned y = 0; y < img->height; ++y) {
        for (unsigned x = 0; x < img->width; ++x) {
            auto pixel = img->getPixel(x,y); //current position
            auto next = img->getPixel(x+1,y); //future pixel

            auto start = ivec2(static_cast<int>(x),static_cast<int>(y));

            if(pixel.a <= alpha_limit && next.a > alpha_limit && find_if(rims.begin(),rims.end(),
                                               [&](const vector<ivec2>& v){
                                                   return find_if(v.begin(),v.end(),[&](const ivec2& p){
                                                       return start == p;
                                                   }) != v.end();
                                               }) == rims.end()){ // check if we are on the rim of an "object" and is part of a rim
                unsigned long long dir = 0;
                std::cout<< std::endl << "new Rim" << std::endl;
                std::cout<< "("<< start.x << "/"<< start.y << ")";
                int count = 0;
                while(img->getPixel(start+directions.at(dir).at(0)).a > alpha_limit
                      || img->getPixel(start+directions.at(dir).at(0)+directions.at(dir).at(1)).a <= alpha_limit) {
                    dir = (dir + 1) % directions.size();
                    count++;
                    assert(count < 8 && "start");
                }

                auto current = start+directions.at(dir).at(0);
                rim.push_back(start);
                auto pixelcount = 0;
                while(current != start){ //Run along the rim till we have a loop
                    count = 0;
                    while(img->getPixel(current+directions.at(dir).at(0)).a > alpha_limit
                          || img->getPixel(current+directions.at(dir).at(0)+directions.at(dir).at(1)).a <= alpha_limit) {
                        dir = (dir+1)%directions.size();
                        count++;
                        assert(count < 8 && "chain");
                    }

                    std::cout<< "->("<< current.x << "/"<< current.y << ")";

                    rim.push_back(current);
                    current = current+directions.at(dir).at(0);
                    pixelcount++;
                    assert(pixelcount < img->width*img->height);
                }

                rims.push_back(rim);
                rim = vector<ivec2>();
            }
        }
    }

    //glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

}

void mainLoop(float dt){
    glClearColor(0.4f,0.6f,0.9f,1.f);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

    renderImage1->Render();

}


int main(int argc, char *argv[]) {

    Util::initStacktrace(argv[0]);
    //std::ifstream is;
    //is.open("test.png",std::ifstream::in | ios_base::binary);
    //std::cout << "png is valid? " << validate(is) << std::endl;
    //is.close();
    glewExperimental = true;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
    glfwMakeContextCurrent(window);

    glewInit();
    init();

    while(!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();

        static auto time = std::chrono::high_resolution_clock::now();
        static float dt = 0.f;
        dt = std::chrono::duration_cast<std::chrono::duration<float>>(time - std::chrono::high_resolution_clock::now()).count();
        mainLoop(dt);
        time = std::chrono::high_resolution_clock::now();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
    glfwTerminate();

    return 0;
}