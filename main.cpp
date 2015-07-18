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

imageStruct* img;
renderImage* renderImage1;

vector<renderRim> rimsToRender;

vector<vector<ivec2>> findRims(float alpha_limit, const imageStruct* img) ;
vector<vector<vec2>> simplyfiyRims(vector<vector<ivec2>> rims, float errorMargin);

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

    vector<vector<ivec2>> rims = findRims(alpha_limit,img);

    for(auto& rim : rims){
        rimsToRender.emplace_back(rim,long(img->width),long(img->height));
    }

    vector<vector<vec2>> simpleRims = simplyfiyRims(rims,1.f);
    for(auto& rim: simpleRims){
        cout << endl << "simple rim" << endl;
        for(auto& v : rim){
            cout<< "("<< v.x << "/"<< v.y << ")->";
        }
        rimsToRender.emplace_back(rim,long(img->width),long(img->height));
    }

    //glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

}

vector<vector<ivec2>> findRims(float alpha_limit, const imageStruct* img) {
    vector<vector<ivec2>> rims;
    vector<ivec2> rim;

    vector<vector<ivec2>> directions; //todo find a method to compute this
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

    //walk over the while image
    for (long y = -1; y < long(img->height); ++y) {
        for (long x = -1; x < long(img->width); ++x) {
            auto pixel = img->getPixel(x,y); //current position
            auto next = img->getPixel(x+1,y); //future pixel

            auto start = ivec2(static_cast<int>(x),static_cast<int>(y));

            //find every possible alpha rim
            if(pixel.a <= alpha_limit && next.a > alpha_limit && find_if(rims.begin(),rims.end(),
                                               [&](const vector<ivec2>& v){
                                                   return find(v.begin(),v.end(),start) != v.end();
                                               }) == rims.end()){ // check if we are on the rim of an "object" and is part of a rim
                unsigned long long dir = 0;
                cout<< endl << "new Rim" << endl;
                cout<< "("<< start.x << "/"<< start.y << ")";
                int count = 0;

                //get a head start
                while(img->getPixel(start+directions.at(dir).at(0)).a > alpha_limit // is where I want to walk still enough transparancy?
                      || img->getPixel(start+directions.at(dir).at(0)+directions.at(dir).at(1)).a <= alpha_limit) { // is rigth from that pixel still an opace pixel?
                    dir = (dir + 1) % directions.size(); // if we didn't found it look into an other direction
                    count++;
                    assert(count < 8 && "start"); // if we looked into all directions and didn't found anything
                }

                auto current = start+directions.at(dir).at(0); // set headstart
                rim.push_back(start);
                auto pixelcount = 0;
                while(current != start){ //Run along the rim till we have a loop

                    //prevent a loop that doesn't end in the start
                    auto found = find(rim.begin(),rim.end(),current);
                    if(found != rim.end()){
                        vector<ivec2> buf;
                        for(;found!=rim.end();found++){
                            buf.push_back(*found);
                        }
                        rim.swap(buf);
                        break;
                    }


                    count = 0;
                    //find next rim pixel
                    while(img->getPixel(current+directions.at(dir).at(0)).a > alpha_limit
                          || img->getPixel(current+directions.at(dir).at(0)+directions.at(dir).at(1)).a <= alpha_limit) {
                        dir = (dir+1)%directions.size();
                        count++;
                        assert(count < 8 && "chain");
                    }

                    cout<< "->("<< current.x << "/"<< current.y << ")";

                    //add found chainlink into rim
                    rim.push_back(current);
                    current = current+directions.at(dir).at(0);
                    pixelcount++;
                    assert(pixelcount < img->width*img->height); // we somehow ended in a loop that is longer than we have pixels
                }

                rims.push_back(rim); // add rim to found rims
                rim = vector<ivec2>(); // an new rim for the next run
            }
        }
    }
    return rims;
}

vector<vec2> simplyfiyRim(vector<ivec2> rim, float errorMargin){
    vector<vec2> erg;
    for(auto& r : rim){
        erg.emplace_back(float(r.x), float(r.y));
    }

    auto noSimplification = false;
    while(!noSimplification){
        noSimplification = true;
        vector<vec2> buf;
        for(size_t i = 0; i < erg.size()-2;){
            auto vec1 = erg.at(i);
            auto vec2 = erg.at(i++);
            auto vec3 = erg.at(i++);

            auto diff1 = vec2-vec1;
            auto diff2 = vec3-vec1;

            auto diffangle = degrees(angle(diff1,diff2));
            //cout << "angle: " << diffangle << endl;
            while(diffangle < errorMargin){
                vec2 = vec3;
                i = (i+1)%erg.size();
                vec3 = erg.at(i);
                diff1 = vec2-vec1;
                diff2 = vec3-vec1;
                diffangle = degrees(angle(diff1,diff2));
            }
            if(find(buf.begin(),buf.end(),vec1) == buf.end()) {
                buf.push_back(vec1);
            }
            if(find(buf.begin(),buf.end(),vec2) == buf.end()) {
                buf.push_back(vec2);
            }
            i--;
        }
        erg.swap(buf);
        buf.clear();
    }
    return erg;
}

vector<vector<vec2>> simplyfiyRims(vector<vector<ivec2>> rims, float errorMargin){
    vector<vector<vec2>> erg;
    for(auto& rim : rims){
        if(rim.size() > 2) {
            erg.push_back(simplyfiyRim(rim, errorMargin));
        }
    }
    return erg;
}

void mainLoop(float dt){
    glClearColor(0.4f,0.6f,0.9f,1.f);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

    renderImage1->Render();
    for(auto& r : rimsToRender){
        r.Render();
    }

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