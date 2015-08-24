//
// Created by Jerry on 20.07.2015.
//

#include "triangleMesh.h"
#include <algorithm>
#include <map>
#include "../glm/gtx/vector_angle.hpp"
#include "../glm/gtx/string_cast.hpp"
#include <iostream>

using namespace glm;
using namespace std;

template<typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

shader triangleMesh::myshader{};
shader triangleMesh::lineShader{};

triangleMesh::triangleMesh(const std::vector<vec2> &posin, long width, long height) : VertexArrayID{0},
                                                                                      vertexbuffer{0},
                                                                                      width{width},
                                                                                      height{height} {
    unsigned int li = 0;
    for (auto &p : posin) {
        pos.emplace_back(p.x,
                         p.y, 0.0f);
        //pos.emplace_back((float(p.x) - width * 0.5f) / float(width) * 2, (float(p.y) - height * 0.5f) / float(height) * 2, 0.0f);
        P.emplace_back(p.x, p.y );
        indexList.push_back(li);
        li++;
    }

    reverse(pos.begin(), pos.end());
    reverse(P.begin(), P.end());

    triangluate();
    initGeom();
    initShader();
}

triangleMesh::triangleMesh(const std::vector<ivec2> &posin, long width, long height) : VertexArrayID{0},
                                                                                       width{width},
                                                                                       height{height},
                                                                                       vertexbuffer{0} {
    unsigned int li = 0;
    for (auto &p : posin) {
        pos.emplace_back(float(p.x),
                        float(p.y), 0.0f);
        P.emplace_back(float(p.x), float(p.y) );
        indexList.push_back(li);
        li++;
    }
    reverse(pos.begin(), pos.end());
    reverse(P.begin(), P.end());
    //reverse(indexList.begin(), indexList.end());


    triangluate();
    initGeom();
    initShader();
}

void triangleMesh::Render() {
    glBindVertexArray(VertexArrayID);
    myshader.loadShader();

    glDrawElements(GL_TRIANGLES,             // mode
                   triangleIndexes.size(),   // count
                   GL_UNSIGNED_INT,          // type
                   (void *) 0);               // element array buffer offset)

    lineShader.loadShader();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES,             // mode
                   triangleIndexes.size(),   // count
                   GL_UNSIGNED_INT,          // type
                   (void *) 0);               // element array buffer offset)
    glPointSize(10.f);
    glDrawElements(GL_POINTS,             // mode
                   triangleIndexes.size(),   // count
                   GL_UNSIGNED_INT,          // type
                   (void *) 0);               // element array buffer offset)

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void triangleMesh::initGeom() {
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);

    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * pos.size(), pos.data(), GL_STATIC_DRAW);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            (3 * 4),                  // stride
            (void *) 0            // array buffer offset
    );

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangleIndexes.size(), triangleIndexes.data(),
                 GL_STATIC_DRAW);

    //elementbuffer;

}

void triangleMesh::initShader() {
    if (!myshader.loaded && !lineShader.loaded) {
        myshader = shader{"#version 330\nin vec4 in_pos;\nuniform mat4 MVP;\nvoid main(){\ngl_Position=MVP*in_pos;\n}\n",
                          "#version 330\nout vec4 out_color;\nvoid main(){\nout_color=vec4(1.0,0.0,0.0,1);\n}\n"};
        lineShader = shader{"#version 330\nin vec4 in_pos;\nuniform mat4 MVP;\nvoid main(){\ngl_Position=MVP*in_pos;\n}\n",
                            "#version 330\nout vec4 out_color;\nvoid main(){\nout_color=vec4(0.0,1.0,0.0,1);\n}\n"};
    }
}

void triangleMesh::triangluate() {


    /*unsigned int li = 0;
    for (auto &vi : pos) {

        //pos.emplace_back((float(p.x) - width * 0.5f) / float(width) * 2, (float(p.y) - height * 0.5f) / float(height) * 2, 0.0f);
        P.push_back(vec2((vi.x * (float(width) * 2) + width * 0.5f)  ,(vi.y* (float(height) * 2)+ height * 0.5f)  ));
        indexList.push_back(li);
        li++;
    }*/

    //reverse(P.begin(), P.end());
    //reverse(indexList.begin(), indexList.end());


    for (auto i = size_t(0); i < indexList.size(); i++) {
        categorize(P, indexList, E, R, C, i);
    }


}

void triangleMesh::revaluate(unsigned int pre, vector<vec2> &P, vector<unsigned int> &indexList,
                             list<unsigned int> &E, vector<unsigned int> &R, vector<unsigned int> &C) {
    auto foundInR = find(R.begin(), R.end(), pre);
    auto foundInE = find(E.begin(), E.end(), pre);

    auto found = find(indexList.begin(), indexList.end(), pre);

    auto preindex = *((found == indexList.begin()) ? indexList.end() - 1 : found - 1);
    auto index = *found;
    auto postindex = *((found + 1 == indexList.end()) ? indexList.begin() : found + 1);


    /*cout << "indexlist: " << endl;
    for (auto &in : indexList) {
        cout << in << " -> ";
    }
    cout << endl;*/

    /*cout << "Ear list: " << endl;
    for (auto &in : E) {
        cout << in << " -> ";
    }
    cout << endl;

    cout << preindex << " -> " << index << " -> " << postindex << endl;*/

    auto &Vi0 = P.at(preindex);
    auto &Vi1 = P.at(index);
    auto &Vi2 = P.at(postindex);

    //cout << to_string(Vi0) << " -> " << to_string(Vi1) << " -> " << to_string(Vi2) << endl;


    if (foundInE != E.end()) {

        for (auto &in : R) {
            auto &p = P.at(in);
            //if (all(equal(Vi0, p)) || all(equal(Vi1, p)) || all(equal(Vi2, p))) continue;
            if (PointInTriangle(p, Vi0, Vi1, Vi2)) {
                E.erase(foundInE);
                break;
            }
        }

    }


    if (foundInR != R.end()) {
        auto p0rel = normalize(Vi1 - Vi0);
        auto p1rel = normalize(Vi2 - Vi1);

        if (180.f - degrees(orientedAngle(p0rel, p1rel)) < 180.f) {
            C.push_back(*foundInR);
            R.erase(foundInR);
        }
    }

    auto foundInC = find(C.begin(), C.end(), pre);
    if (foundInC != C.end()) {
        bool isEar = true;
        for (auto &in : R) {
            auto &p = P.at(in);
            if (all(equal(Vi0, p)) || all(equal(Vi1, p)) || all(equal(Vi2, p))) continue;
            if (PointInTriangle(p, Vi0, Vi1, Vi2)) {
                isEar = false;
                break;
            }
        }

        if (isEar && find(E.begin(), E.end(), *foundInC) == E.end()) {
            //cout << " new ear" << endl;
            E.push_back(*foundInC);
        }
    }


}

void triangleMesh::categorize(vector<vec2> &P, vector<unsigned int> &indexList, list<unsigned int> &E,
                              vector<unsigned int> &R,
                              vector<unsigned int> &C, unsigned int index) {

    //unsigned int &catVertIndex = indexList.at((index) % size);
    auto found = find(indexList.begin(), indexList.end(), index);


    auto preindex = *((found == indexList.begin()) ? indexList.end() - 1 : found - 1);
    auto inindex = *found;
    auto postindex = *((found + 1 == indexList.end()) ? indexList.begin() : found + 1);

    //cout << preindex << " -> " << inindex << " -> " << postindex << endl;

    auto &Vi0 = P.at(preindex);
    auto &Vi1 = P.at(inindex);
    auto &Vi2 = P.at(postindex);

    //cout << to_string(Vi0) << " -> " << to_string(Vi1) << " -> " << to_string(Vi2) << endl;

    bool isEar = false;


    auto p0rel = normalize(Vi1 - Vi0);
    auto p1rel = normalize(Vi2 - Vi1);

    auto deg = 180.f - degrees(orientedAngle(p0rel, p1rel));
   // cout << deg << " ";
    if (deg < 180.f) {
     //   cout << "Convex" << endl;
        C.push_back(index);

        isEar = true;
        for (auto &p : P) {
            //auto &p = P.at(in);

            if (all(equal(Vi0, p)) || all(equal(Vi1, p)) || all(equal(Vi2, p))) continue;
            if (PointInTriangle(p, Vi0, Vi1, Vi2)) {
                isEar = false;
                break;
            }
        }

    } else {//if (deg >= 180.f) {
     //   cout << "reflex" << endl;

        R.push_back(index);
    }

    if (isEar) {
     //   cout << "Ear" << endl;
        E.push_back(index);
    }
}

bool triangleMesh::Step() {

    if (!E.empty()) {
        auto i = E.front();
        E.pop_front();

        auto found = find(indexList.begin(), indexList.end(), i);
        if (found != indexList.end()) {
            auto pre = *((found == indexList.begin()) ? indexList.end() - 1 : found - 1);
            auto cur = *found;
            auto next = *((found + 1 == indexList.end()) ? indexList.begin() : found + 1);

            indexList.erase(found);

            triangleIndexes.push_back(pre);
            triangleIndexes.push_back(cur);
            triangleIndexes.push_back(next);

            revaluate(pre, P, indexList, E, R, C);
            revaluate(next, P, indexList, E, R, C);
        }
    }
    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangleIndexes.size(), triangleIndexes.data(),
                 GL_STATIC_DRAW);

    return !E.empty();
}
