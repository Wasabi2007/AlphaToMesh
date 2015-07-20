//
// Created by Jerry on 20.07.2015.
//

#include "triangleMesh.h"
#include <algorithm>

using namespace glm;
using namespace std;

shader triangleMesh::myshader{};

triangleMesh::triangleMesh(const std::vector<vec2> &posin, long width, long height) : VertexArrayID{0}, vertexbuffer{0} {

    for(auto& p : posin) {
        pos.emplace_back((float(p.x)-width*0.5f)/float(width)*2, (float(p.y)-height*0.5f)/float(height)*2,0.0f);
    }

    triangluate();
    initGeom();
    initShader();
}

triangleMesh::triangleMesh(const std::vector<ivec2> &posin, long width, long height) : VertexArrayID{0}, vertexbuffer{0} {
    for(auto& p : posin) {
        pos.emplace_back((p.x-width*0.5f)/width*2, (p.y-height*0.5f)/height*2,0.0f);
    }

    triangluate();
    initGeom();
    initShader();
}

void triangleMesh::Render() {
    glBindVertexArray(VertexArrayID);
    myshader.loadShader();
    glDrawElements(GL_TRIANGLES,      // mode
                   triangleIndexes.size(),    // count
                   GL_UNSIGNED_INT,   // type
                   (void*)0) ;          // element array buffer offset)
}

void triangleMesh::initGeom() {
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);

    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*pos.size(), pos.data(), GL_STATIC_DRAW);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            (3*4),                  // stride
            (void*)0            // array buffer offset
    );

    glGenBuffers(1,&elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*triangleIndexes.size(),triangleIndexes.data(),GL_STATIC_DRAW);

    //elementbuffer;

}

void triangleMesh::initShader() {
    if(!myshader.loaded){
        myshader = shader{"#version 420\nin vec4 in_pos;\nvoid main(){\ngl_Position=in_pos;\n}\n",
                          "#version 420\nout vec4 out_color;\nvoid main(){\nout_color=vec4(0.5,0.5,0.5,1);\n}\n"};
    }
}

void triangleMesh::triangluate() {
    vector<pointIndex> Triangles;

    vector<pointIndex> V;
    vector<pointIndex> L;
    unsigned int count = -1;
    for(auto& p : pos){
        V.emplace_back(p,count++);
    }

    sort (V.begin(), V.end(), [](const pointIndex& a, const pointIndex& b){
        return a.pos.x > b.pos.x;
    });

    L.push_back(V.back());
    V.pop_back();

    L.push_back(V.back());
    V.pop_back();

    while(!V.empty()){
        auto p = V.back();
        V.pop_back();

        bool opposit = false;
    }

}
