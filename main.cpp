#include <iostream>
#include <fstream>      // std::filebuf
#include <vector>
#include <iomanip>
#include <chrono>
#include <algorithm>

#define GLEW_STATIC

//#define PNG_DEBUG 3
//#include <lodepng/lodepng.h>
#include "glm/glm.hpp"
#include "glm/gtx/perpendicular.hpp"
#include "lodepng/lodepng.h"

#include "glew/include/GL/glew.h"
#include "glfw/include/GLFW/glfw3.h"

#include <stdexcept>

#define STACKTRACE

#include "stacktrace.hpp"


using namespace std;
using namespace glm;

struct imageStruct {
    std::vector<unsigned char> image; //the raw pixels
    const unsigned width, height;

    imageStruct(const vector<unsigned char> &image, unsigned int width, unsigned int height) : image(image),
                                                                                               width(width),
                                                                                               height(height) { }

    vec4 getPixel(unsigned x, unsigned y) {
        if(x < 0 || y < 0 || x > width || y > height){
            return vec4(0.f, 0.f, 0.f, 0.f);
        }
        auto index = y * width * 4 + x * 4;

        auto r = float(image.at(index)) / 255.f;
        auto g = float(image.at(index + 1)) / 255.f;
        auto b = float(image.at(index + 2)) / 255.f;
        auto a = float(image.at(index + 3)) / 255.f;

        return vec4(r, g, b, a);
    }

    vec4 getPixel(ivec2 pos){
        if(pos.x < 0 || pos.y < 0 || pos.x > width || pos.y > height){
            return vec4(0.f, 0.f, 0.f, 0.f); // retun invisible if we step over bounderays
        }

        return  getPixel(unsigned(pos.x),unsigned(pos.y));
    }

};

imageStruct* decode(const char *filename) {
    std::vector<unsigned char> png;
    std::vector<unsigned char> image; //the raw pixels
    std::vector<unsigned char> bufimage;
    unsigned width, height;

    //load and decode
    lodepng::load_file(png, filename);
    unsigned error = lodepng::decode(bufimage, width, height, png);

    for(auto i = 0; i < width*height;++i){ // converting into opengl frendly pixel layout
        auto a = bufimage.back();
        bufimage.pop_back();
        auto b = bufimage.back();
        bufimage.pop_back();
        auto g = bufimage.back();
        bufimage.pop_back();
        auto r = bufimage.back();
        bufimage.pop_back();

        image.push_back(r);
        image.push_back(g);
        image.push_back(b);
        image.push_back(a);
    }

    //if there's an error, display it
    if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
    return new imageStruct(image, width, height);
}

imageStruct* img = nullptr;
GLuint VertexArrayID{0};
GLuint vertexbuffer{0};
GLuint elementbuffer{0};

GLuint VertexShader{0};
GLuint FragmentShader{0};
GLuint ProgrammShader{0};

GLuint Texture{0};


void initGeom() {
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);


    // An array of 3 vectors which represents 3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,//Position
            0.0f, 0.0f, //UV

            1.0f, -1.0f, 0.0f,//Position
            1.0f, 0.0f, //UV

            1.0f,  1.0f, 0.0f,//Position
            1.0f, 1.0f, //UV

            -1.0f,  1.0f, 0.0f,//Position
            0.0f, 1.0f, //UV
    };


    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);

    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            (3*4)+8,                  // stride
            (void*)0            // array buffer offset
    );

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            (3*4)+8,                  // stride
            (void*)(3*4)            // array buffer offset
    );

    std::vector<unsigned int> indices;

// fill "indices" as needed

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);


// Generate a buffer for the indices
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void initShader(){
    // Create the shaders
    VertexShader = glCreateShader(GL_VERTEX_SHADER);
    FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    VertexShaderCode += "#version 420\n";
    VertexShaderCode += "in vec4 in_pos;\n";
    VertexShaderCode += "in vec2 in_uv;\n";
    VertexShaderCode += "out vec2 var_uv;\n";
    VertexShaderCode += "void main(){\n";
    VertexShaderCode += "gl_Position=in_pos;\n";
    VertexShaderCode += "var_uv=in_uv;\n";
    VertexShaderCode += "}\n";


    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    FragmentShaderCode += "#version 420\n";
    FragmentShaderCode += "in vec2 var_uv;\n";
    FragmentShaderCode += "uniform sampler2D tex;\n";
    FragmentShaderCode += "out vec4 out_color;\n";
    FragmentShaderCode += "void main(){\n";
    FragmentShaderCode += "out_color=texture(tex,var_uv);\n";
    FragmentShaderCode += "}\n";

    GLint Result = GL_FALSE;
    int InfoLogLength;

    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShader, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShader);

    // Check Vertex Shader
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShader, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Compile Fragment Shader
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShader, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShader);

    // Check Fragment Shader
    glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShader, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    // Link the program
    fprintf(stdout, "Linking program\n");
    ProgrammShader = glCreateProgram();
    glAttachShader(ProgrammShader, VertexShader);
    glAttachShader(ProgrammShader, FragmentShader);


    glBindAttribLocation(ProgrammShader, 1, "in_pos");//Attribut Nummer 1 soll in in_pos im Vertex Shader zur Verfügung stehen
    glBindAttribLocation(ProgrammShader, 2, "in_uv");//Attribut Nummer 3 soll in in_uv im Vertex Shader zur Verfügung stehen
    glBindFragDataLocation(ProgrammShader, 0, "out_color");//out_color ist Farbe 0 (die in dem Framebuffer geschrieben werden)


    glLinkProgram(ProgrammShader);

    // Check the program
    glGetProgramiv(ProgrammShader, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgrammShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( InfoLogLength );
    glGetProgramInfoLog(ProgrammShader, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glUseProgram(ProgrammShader);
    glUniform1i(glGetUniformLocation(ProgrammShader, "tex"), 0);
}

void initTexture(){
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);

    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void init(){
    img = decode("test.png");

    std::cout << std::fixed << std::setprecision(1);
    /*for (unsigned y = 0; y < img->height; ++y) {
        for (unsigned x = 0; x < img->width; ++x) {
            std::cout << (img->getPixel(x, y).a>0.5?"#":(img->getPixel(x, y).a>0.25?"+":(img->getPixel(x, y).a>0.01?"-":" ")));
        }
        std::cout << std::endl;
    }*/

    auto alpha_limit = 0.5f;

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

    auto run = true;

    //todo what happens when I start at an alpha value or hit an image border
    for (unsigned y = 0; y < img->height && run; ++y) {
        for (unsigned x = 0; x < img->width-1 && run; ++x) {
            auto pixel = img->getPixel(x,y); //current position
            auto next = img->getPixel(x+1,y); //future pixel
            if(next.a > alpha_limit){ // check if we are on the rim of an "object"
                run = false;
                auto start = ivec2(static_cast<int>(x),static_cast<int>(y));
                unsigned long long dir = 0;
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
                }
            }
        }
    }

    //glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    initGeom();
    initShader();
    initTexture();
}

void mainLoop(float dt){
    glClearColor(0.4f,0.6f,0.9f,1.f);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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