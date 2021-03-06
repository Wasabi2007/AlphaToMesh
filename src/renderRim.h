//
// Created by Jerry on 18.07.2015.
//

#ifndef ALPHATOMESH_RENDERRIM_H
#define ALPHATOMESH_RENDERRIM_H

#include <vector>
#include "../glew/include/GL/glew.h"
#include "../glm/glm.hpp"
#include "shader.h"

class renderRim {
private:
    std::vector<glm::vec3> pos;

    GLuint VertexArrayID;
    GLuint vertexbuffer;

    bool renderVerts;

public:

    static shader myshader;
    renderRim(const std::vector<glm::vec2>& posin, long width, long height);
    renderRim(const std::vector<glm::ivec2>& posin, long width, long height);

    void Render();


    bool isRenderVerts() const {
        return renderVerts;
    }

    void setRenderVerts(bool renderVerts) {
        renderRim::renderVerts = renderVerts;
    }

private:
    void initGeom();
    void initShader();
};


#endif //ALPHATOMESH_RENDERRIM_H
