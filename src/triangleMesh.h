//
// Created by Jerry on 20.07.2015.
//

#ifndef ALPHATOMESH_TRIANGLEMESH_H
#define ALPHATOMESH_TRIANGLEMESH_H


#include "../glew/include/GL/glew.h"
#include "shader.h"
#include "../glm/glm.hpp"

class triangleMesh {
    enum VertexType{
        DEFAULT,
        START,
        END,
        SPLIT,
        MERGE
    };

    struct pointIndex{
        glm::vec3 pos;
        unsigned int index;
        VertexType vertexType;

        pointIndex(const glm::vec3 &pos, unsigned int index, VertexType vertexType = VertexType::DEFAULT) : pos(pos), index(index) , vertexType(vertexType) { }
    };

    struct edge{
        pointIndex p1;
        pointIndex p2;

        edge(const pointIndex &p1, const pointIndex &p2) : p1(p1), p2(p2) { }
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
    std::vector<pointIndex> triangluateMonoton(std::vector<pointIndex> MontonPolygon);
};


#endif //ALPHATOMESH_TRIANGLEMESH_H
