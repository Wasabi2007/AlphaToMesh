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

imageStruct* img;
renderImage* renderImage1;

vector<vector<ivec2>> findRims(float alpha_limit, const imageStruct* img) ;
vector<vector<vec2>> simplyfiyRims(vector<vector<ivec2>> rims, float errorMargin);




string filename;

float alpha_limit = 0.9f;
float errorMarginDegree = 1.f;

bool stepp = false;

void init(vector<renderRim>& rimsToRender,vector<triangleMesh>& meshToRender){
    img = imageStruct::load(filename.c_str());
    renderImage1 = new renderImage(*img);

    std::cout << std::fixed << std::setprecision(1);
    /*for (unsigned y = 0; y < img->height; ++y) {
        for (unsigned x = 0; x < img->width; ++x) {
            std::cout << (img->getPixel(x, y).a>0.5?"#":(img->getPixel(x, y).a>0.25?"+":(img->getPixel(x, y).a>0.01?"-":" ")));
        }
        std::cout << std::endl;
    }*/



    vector<vector<ivec2>> rims = findRims(alpha_limit,img);

    for(auto& rim : rims){
        rimsToRender.emplace_back(rim,long(img->width),long(img->height));
    }

    vector<vector<vec2>> simpleRims = simplyfiyRims(rims,errorMarginDegree);
    for(auto& rim: simpleRims){
        /*cout << endl << "simple rim" << endl;
        for(auto& v : rim){
            cout<< "("<< v.x << "/"<< v.y << ")->";
        }*/
        //rimsToRender.emplace_back(rim,long(img->width),long(img->height));
        meshToRender.emplace_back(rim,long(img->width),long(img->height));
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

    vector<vector<bool> > partOfChain; // save witch pixels are already part of a chain. Cheaper then looking throug all existing rims.
    partOfChain.resize(img->height+2); // +2 to compensate the grap over the imageborder
    for (int i = 0; i < img->height+2; ++i)
        partOfChain[i].resize(img->width+2);

    //walk over the while image
    for (long y = -1; y <= long(img->height); ++y) {
        for (long x = -1; x <= long(img->width); ++x) {
            if(partOfChain[y+1][x+1]) continue; // skip pixel that is already in a chain

            auto pixel = img->getPixel(x,y); //current position
            auto next = img->getPixel(x+1,y); //future pixel

            auto start = ivec2(static_cast<int>(x),static_cast<int>(y));

            //find every possible alpha rim
            if(pixel.a <= alpha_limit && next.a > alpha_limit) { // check if we are on the rim of an "object" and is part of a rim
                unsigned long long dir = 0;
                cout << endl << "new Rim" << endl;
                //cout<< "("<< start.x << "/"<< start.y << ")";
                int count = 0;

                bool pixelOnly = false;

                //get a head start
                while (img->getPixel(start + directions.at(dir).at(0)).a >
                       alpha_limit // is where I want to walk still enough transparancy?
                       || img->getPixel(start + directions.at(dir).at(0) + directions.at(dir).at(1)).a <=
                          alpha_limit) { // is rigth from that pixel still an opace pixel?
                    dir = (dir + 1) % directions.size(); // if we didn't found it look into an other direction
                    count++;
                    if (count > 7) { // only one pixel transparent
                        pixelOnly = true;
                        break;
                    }
                    assert(count < 8 && "start"); // if we looked into all directions and didn't found anything
                }
                if (!pixelOnly) {

                    auto current =
                            start + directions.at(dir).at(0); // set headstart so that the search dont end imdiataly;
                    rim.push_back(start);
                    partOfChain[start.y + 1][start.x + 1] = true;
                    partOfChain[current.y + 1][current.x + 1] = true;
                    auto pixelcount = 0;
                    //todo maybe change to a do while
                    while (current != start) { //Run along the rim till we have a loop

                        //prevent a loop that doesn't end in the start
                        auto found = find(rim.begin(), rim.end(), current);
                        if (found != rim.end()) {

                            for (auto &p : rim) {
                                partOfChain[p.y + 1][p.x + 1] = false;
                            }

                            vector<ivec2> buf;
                            for (; found != rim.end(); found++) {
                                buf.push_back(*found);
                                partOfChain[(*found).y + 1][(*found).x + 1] = true;
                            }
                            rim.swap(buf);
                            break;
                        }


                        count = 0;
                        //find next rim pixel
                        auto maindir = current + directions.at(dir).at(0);
                        auto maindirNormal = current + directions.at(dir).at(0) + directions.at(dir).at(1);

                        auto dirFound = true;

                        while (img->getPixel(maindir).a > alpha_limit
                               || img->getPixel(maindirNormal).a <= alpha_limit
                               || partOfChain[maindir.y + 1][maindir.x + 1]) { // first run ignoring chain parts
                            dir = (dir + 1) % directions.size();

                            maindir = current + directions.at(dir).at(0);
                            maindirNormal = current + directions.at(dir).at(0) + directions.at(dir).at(1);

                            count++;
                            if (count > 8) {
                                dirFound = false;
                                break;
                            }

                            //assert(count < 8 && "chain");
                        }

                        if (!dirFound) { // now with already pixel that are already part of a chain
                            count = 0;
                            while (img->getPixel(maindir).a > alpha_limit
                                   || img->getPixel(maindirNormal).a <= alpha_limit) {
                                dir = (dir + 1) % directions.size();

                                maindir = current + directions.at(dir).at(0);
                                maindirNormal = current + directions.at(dir).at(0) + directions.at(dir).at(1);

                                count++;
                                assert(count < 8 && "chain");
                            }
                        }

                        //cout<< "->("<< current.x << "/"<< current.y << ")";

                        //add found chainlink into rim
                        rim.push_back(current);
                        partOfChain[current.y + 1][current.x + 1] = true;
                        current = current + directions.at(dir).at(0);
                        pixelcount++;
                        assert(pixelcount < img->width *
                                            img->height); // we somehow ended in a loop that is longer than we have pixels
                    }

                    rims.push_back(rim); // add rim to found rims
                    rim = vector<ivec2>(); // an new rim for the next run
                }
            }
        }
    }
    return rims;
}

vector<vec2> simplyfiyRim(vector<vec2> rim, float errorMargin){
    //DouglasPeucker
    // Find the point with the maximum distance

    vector<vec2> erg;

    auto dmax = 0.f;
    auto index = size_t(0);
    auto end = rim.size()-1;
    for(auto i = size_t(1); i < ( end ); ++i) {
        auto line = rim[end]-rim[0];
        auto relpoint = rim[i]-rim[0];
        auto distToLine = (dot(relpoint,line)/dot(line,line)*line)-relpoint;
        auto d = length(distToLine);
        if ( d > dmax ) {
            index = i;
            dmax = d;
        }
    }
    // If max distance is greater than epsilon, recursively simplify
    if ( dmax > errorMargin ) {
        // Recursive call
        auto recResults1 = simplyfiyRim(vector<vec2>(rim.begin(),rim.begin()+index), errorMargin);
        auto recResults2 = simplyfiyRim(vector<vec2>(rim.begin()+index,rim.end()), errorMargin);

        // Build the result list
        erg = recResults1;//{recResults1[1...length(recResults1)-1] recResults2[1...length(recResults2)]}
        erg.insert(erg.end(),recResults2.begin(),recResults2.end());
    } else {
        erg = vector<vec2>();
        erg.push_back(rim[0]);
        erg.push_back(rim[end]);
    }
    // Return the result

    return erg;
}

vector<vec2> simplyfiyRim(vector<ivec2> rim, float errorMargin) {
    vector<vec2> erg;
    for (auto &r : rim) {
        erg.emplace_back(float(r.x), float(r.y));
    }
    return simplyfiyRim(erg,errorMargin);
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

void mainLoop(float dt,vector<renderRim>& rimsToRender,vector<triangleMesh>& meshToRender){
    glClearColor(0.4f,0.6f,0.9f,1.f);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

    glLineWidth(5.0f);
    //glPointSize(50.f);
    renderImage1->Render();

    for(auto& r : meshToRender){
        r.Render();
    }

    for(auto& r : rimsToRender){
        //r.Render();
    }

    if(stepp){
        stepp = false;
        for(auto& r : meshToRender){
            r.Step();
        }
    }


}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && mods != GLFW_MOD_CONTROL)
        stepp = true;

    if (key == GLFW_KEY_SPACE && mods == GLFW_MOD_CONTROL)
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

    if(argc > 1){
        filename = argv[1];
    } else {
        filename = "test0.png";
    }

    if(argc > 2){
        alpha_limit = stof(argv[2]);
    }
    if(argc > 3){
        errorMarginDegree = stof(argv[3]);
    }


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800*2, 600*2, "AlphaToMesh", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    glewExperimental = GL_TRUE;
    glewInit();
    init(rimsToRender,meshToRender);

    float lineWidth[2];
    glGetFloatv(GL_LINE_WIDTH_RANGE, lineWidth);

    //cout << lineWidth[0] << " " << lineWidth[1] << endl;

    while(!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();

        static auto time = std::chrono::high_resolution_clock::now();
        static float dt = 0.f;
        dt = std::chrono::duration_cast<std::chrono::duration<float>>(time - std::chrono::high_resolution_clock::now()).count();
        mainLoop(dt,rimsToRender,meshToRender);
        time = std::chrono::high_resolution_clock::now();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
    glfwTerminate();

    return 0;
}