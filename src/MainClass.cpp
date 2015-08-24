//
// Created by jerry on 24.08.15.
//

#include <gtc/matrix_transform.hpp>
#include <gtx/string_cast.hpp>
#include "MainClass.hpp"

bool MainClass::mvpreload = false;
int MainClass::width = 800;
int MainClass::height = 600;

void TW_CALL initRims(void *data) {
    MainClass *aClass = (MainClass *) data;

    Rim r{aClass->alpha_limit, aClass->img.get(), aClass->errorMarginDegree};
    aClass->init(r);
    aClass->WindowSize();
}

void TW_CALL loadImage(void *data) {
    MainClass *aClass = (MainClass *) data;
    aClass->img = std::unique_ptr<imageStruct>(imageStruct::load(aClass->filename.c_str()));
    aClass->renderImage1 = std::make_unique<renderImage>(*aClass->img);
    aClass->rimsToRender.clear();
    aClass->meshToRender.clear();
    aClass->WindowSize();
}

void TW_CALL doStep(void *data) {
    MainClass *aClass = (MainClass *) data;
    aClass->stepp = true;
}


void TW_CALL doAutoStep(void *data) {
    MainClass *aClass = (MainClass *) data;
    aClass->autoStep = !aClass->autoStep;
}


void WindowSizeCB(GLFWwindow *_window, int width, int height) {
    //glViewport(0, 0, width, height);
    //std::cout << "Callback?" << std::endl;

    MainClass::width = width;
    MainClass::height = height;
    MainClass::mvpreload = true;

    // Send the new window size to AntTweakBar
    TwWindowSize(width, height);
}

MainClass::MainClass(const string &filename,float alpha_limit, float errorMarginDegree) : alpha_limit(alpha_limit),
                                                                                errorMarginDegree(
                                                                                        errorMarginDegree),
                                                                                filename(filename),
                                                                                          stepp(false),
                                                                                          canStep(true),
                                                                                          autoStep(false),
                                                                                          timeCount(0.f),
                                                                                          stepTime(1.f)
{

}

void MainClass::initTW(){
    auto window = glfwGetCurrentContext();
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(window,&width,&height);


    TwDefine(" GLOBAL fontscaling=3 ");
    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(width, height);

    animationBar = TwNewBar("PlayControlle");
    TwAddButton(animationBar,"Step",doStep,this," label='Step' ");
    TwAddButton(animationBar,"AutoStep",doAutoStep,this," label='Auto Step' ");
    TwAddVarRW(animationBar, "speed", TW_TYPE_FLOAT, &stepTime,
               " label='Step speed' min=0 max=10 step=0.1");


    allgoBar = TwNewBar("AlgoControlle");

    TwCopyStdStringToClientFunc([](std::string& destinationClientString, const std::string& sourceLibraryString)
    {
        // Copy the content of souceString handled by the AntTweakBar library to destinationClientString handled by your application
        destinationClientString = sourceLibraryString;
    });

    TwAddVarRW(allgoBar,"Filename",TW_TYPE_STDSTRING,&this->filename,"");
    TwAddButton(allgoBar,"Loadfile",loadImage,this," label='Loadfile' ");

    TwAddVarRW(allgoBar,"Alphaschwellwert",TW_TYPE_FLOAT, &alpha_limit," label='Alpha Limit' min=0 max=1 step=0.01");
    TwAddVarRW(allgoBar,"Simplification",TW_TYPE_FLOAT, &errorMarginDegree," label='Simplification' min=0 step=0.1");

    TwAddButton(allgoBar,"Find Rims",initRims,this," label='Find Rims' ");

    glfwSetFramebufferSizeCallback(window,WindowSizeCB);
}

void MainClass::init(Rim r) {
    rimsToRender.clear();
    meshToRender.clear();


    //std::cout << std::fixed << std::setprecision(1);
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
}

void MainClass::mainLoop(float dt) {
    glClearColor(0.4f, 0.6f, 0.9f, 1.f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if(MainClass::mvpreload){
        MainClass::mvpreload = false;
        WindowSize();
    }

    glLineWidth(5.0f);
    //glPointSize(50.f);
    if(renderImage1)
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
            canStep = r.Step();
        }
    }

    if(autoStep && timeCount >= stepTime){
        timeCount = 0.f;
        for (auto &r : meshToRender) {
            canStep = r.Step();
        }
    }
    timeCount += dt;
    //cout << "timeCount: " << timeCount << endl;
}

void MainClass::WindowSize() {
    glViewport(0, 0, MainClass::width, MainClass::height);

    float wsize = float(img->width)/float(MainClass::width);
    float hsize = float(img->height)/float(MainClass::height);
    float size = fmax(wsize,hsize);

    mat4 mvp{1};
    mvp *= glm::ortho(0.f,float(MainClass::width)*size,0.f,float(MainClass::height)*size);
    //std::cout << "width: " << width << "height: " << height << std::endl;

    //std::cout << glm::to_string(mvp) << std::endl;

    renderRim::myshader.mvp = mvp;
    renderRim::myshader.reloadMVP();

    triangleMesh::myshader.mvp = mvp;
    triangleMesh::myshader.reloadMVP();

    triangleMesh::lineShader.mvp = mvp;
    triangleMesh::lineShader.reloadMVP();

    renderImage1->reloadMVP(mvp);

    // Send the new window size to AntTweakBar
    //TwWindowSize(width, height);
}
