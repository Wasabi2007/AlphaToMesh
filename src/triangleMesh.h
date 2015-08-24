//
// Created by Jerry on 20.07.2015.
//

#ifndef ALPHATOMESH_TRIANGLEMESH_H
#define ALPHATOMESH_TRIANGLEMESH_H


#include "../glew/include/GL/glew.h"
#include "shader.h"
#include "../glm/glm.hpp"
#include <list>

class triangleMesh {
private:
    std::vector<glm::vec3> pos;
    std::vector<unsigned int> triangleIndexes;

    GLuint VertexArrayID;
    GLuint vertexbuffer;

    GLuint elementbuffer;

    long width;

    long height;
    std::vector<glm::vec2> P;


    std::vector<unsigned int> indexList;
    std::list<unsigned int> E; //List of ears


    std::vector<unsigned int> R; //List of reflex verts
    std::vector<unsigned int> C; //List of convex verts
public:
    static shader myshader;
    static shader lineShader;
    triangleMesh(const std::vector<glm::vec2>& posin, long width, long height);
    triangleMesh(const std::vector<glm::ivec2>& posin, long width, long height);

    void Render();
    bool Step();
private:
    void initGeom();
    void initShader();
    void triangluate();

    float sign (glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
    {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }

    bool PointInTriangle (glm::vec2 pt, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3)
    {
        bool b1, b2, b3;

        b1 = sign(pt, v1, v2) < 0.0f;
        b2 = sign(pt, v2, v3) < 0.0f;
        b3 = sign(pt, v3, v1) < 0.0f;

        return ((b1 == b2) && (b2 == b3));
    }

    void categorize(std::vector<glm::vec2> &P,std::vector<unsigned int>& indexList, std::list<unsigned int> &E, std::vector<unsigned int> &R,
                    std::vector<unsigned int> &C, unsigned int index);

    void revaluate(unsigned int pre, std::vector<glm::vec2> &P,
                   std::vector<unsigned int> &indexList, std::list<unsigned int> &E, std::vector<unsigned int> &R,
                   std::vector<unsigned int> &C);
};


#endif //ALPHATOMESH_TRIANGLEMESH_H
