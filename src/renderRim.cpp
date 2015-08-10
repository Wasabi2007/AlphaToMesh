//
// Created by Jerry on 18.07.2015.
//


#include "renderRim.h"
#include <string>

shader renderRim::myshader{};

renderRim::renderRim(const std::vector<glm::ivec2>& posin, long width, long height):
        VertexArrayID{0},vertexbuffer{0}
        {

    for(auto& p : posin) {
        pos.emplace_back((float(p.x)-width*0.5f)/float(width)*2, (float(p.y)-height*0.5f)/float(height)*2,0.0f);
    }

    initGeom();
    initShader();
}

renderRim::renderRim(const std::vector<glm::vec2>& posin, long width, long height):
        VertexArrayID{0},vertexbuffer{0}{

    for(auto& p : posin) {
        pos.emplace_back((p.x-width*0.5f)/width*2, (p.y-height*0.5f)/height*2,0.0f);
    }

    initGeom();
    initShader();
}

void renderRim::Render() {
    glBindVertexArray(VertexArrayID);
    //glLineWidth(50.f);
    myshader.loadShader();
    glDrawArrays(GL_LINE_LOOP,0, GLsizei( pos.size()));
}

void renderRim::initGeom() {
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);

    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*pos.size(), pos.data(), GL_STATIC_DRAW);

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

}


void renderRim::initShader() {
    if(!myshader.loaded){
        myshader = shader{"#version 330\nin vec4 in_pos;\nvoid main(){\ngl_Position=in_pos;\n}\n",
                  "#version 330\nout vec4 out_color;\nvoid main(){\nout_color=vec4(0,1,0,1);\n}\n"};
    }
}
