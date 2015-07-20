//
// Created by Jerry on 20.07.2015.
//

#ifndef ALPHATOMESH_TRIANGLEMESH_H
#define ALPHATOMESH_TRIANGLEMESH_H


#include "../glew/include/GL/glew.h"
#include "shader.h"
#include "../glm/glm.hpp"

class triangleMesh {
    struct pointIndex{
        glm::vec3 pos;
        unsigned int index;

        pointIndex(const glm::vec3 &pos, unsigned int index) : pos(pos), index(index) { }
    };

private:
    std::vector<glm::vec3> pos;
    std::vector<unsigned int> triangleIndexes;

    GLuint VertexArrayID;
    GLuint vertexbuffer;

    GLuint elementbuffer;

    static shader myshader;
public:
    triangleMesh(const std::vector<glm::vec2>& posin, long width, long height);
    triangleMesh(const std::vector<glm::ivec2>& posin, long width, long height);

    void Render();
private:
    void initGeom();
    void initShader();
    void triangluate();
};


#endif //ALPHATOMESH_TRIANGLEMESH_H
