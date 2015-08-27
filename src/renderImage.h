//
// Created by Jerry on 18.07.2015.
//

#ifndef ALPHATOMESH_RENDERIMAGE_H
#define ALPHATOMESH_RENDERIMAGE_H


#include "../glew/include/GL/glew.h"
#include "imageStruct.hpp"

class renderImage {
private:
    imageStruct* img;
    GLuint VertexArrayID;
    GLuint vertexbuffer;
    GLuint elementbuffer;

    GLuint VertexShader;
    GLuint FragmentShader;
    GLuint ProgrammShader;

    GLuint Texture;

    mat4 MVP;
public:
    renderImage(const char* filename);
    renderImage(imageStruct* img);
    ~renderImage();

    void Render();
    void reloadMVP(mat4 mvp);
private:
    void initGeom();
    void initShader();

    void initTexture();
};


#endif //ALPHATOMESH_RENDERIMAGE_H
