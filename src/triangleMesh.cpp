//
// Created by Jerry on 20.07.2015.
//

#include "triangleMesh.h"
#include <algorithm>
#include <queue>
#include <map>
#include "../glm/gtx/vector_angle.hpp"

using namespace glm;
using namespace std;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

shader triangleMesh::myshader{};

triangleMesh::triangleMesh(const std::vector<vec2> &posin, long width, long height) : VertexArrayID{0}, vertexbuffer{0} {

    for(auto& p : posin) {
        pos.emplace_back((float(p.x)-width*0.5f)/float(width)*2, (float(p.y)-height*0.5f)/float(height)*2,0.0f);
    }

    reverse(pos.begin(),pos.end());

    triangluate();
    initGeom();
    initShader();
}

triangleMesh::triangleMesh(const std::vector<ivec2> &posin, long width, long height) : VertexArrayID{0}, vertexbuffer{0} {
    for(auto& p : posin) {
        pos.emplace_back((p.x-width*0.5f)/width*2, (p.y-height*0.5f)/height*2,0.0f);
    }
    reverse(pos.begin(),pos.end());


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
                   (void*)0) ;               // element array buffer offset)
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

    vector<edge> edges;

    static auto comp = [](const pointIndex& a, const pointIndex& b){
        return (a.pos.y == b.pos.y?a.pos.x < b.pos.x:a.pos.y < b.pos.y);
    };

    //std::deq <pointIndex,std::vector<pointIndex>,decltype(comp)> Q(comp); // y sortet pointlist

    std::vector<pointIndex> P; // normal pointlist used for building the edges

    for(unsigned index = 0; index < pos.size(); ++index){

        auto& p0 = pos[(index-1+pos.size())%pos.size()];
        auto& p = pos[index];
        auto& p1 = pos[(index+1)%pos.size()];


        auto p0rel = p0 - p;
        auto p1rel = p1 - p;

        VertexType typ = VertexType::DEFAULT ;

        if(p0.y < p.y && p1.y < p.y && degrees(angle(p0rel,p1rel)) < 180){
            typ = VertexType::START;
        } else if(p0.y > p.y && p1.y > p.y && degrees(angle(p0rel,p1rel)) < 180){
            typ = VertexType::END;
        }else if(p0.y < p.y && p1.y < p.y && degrees(angle(p0rel,p1rel)) > 180){
            typ = VertexType::SPLIT;
        }else if(p0.y > p.y && p1.y > p.y && degrees(angle(p0rel,p1rel)) > 180){
            typ = VertexType::MERGE;
        }
        //Q.emplace(p,index,typ);
        P.emplace_back(p,index,typ);
    }

    std::vector<pointIndex> Q;
    std::copy(P.begin(),P.end(),Q.begin());

    std::sort(Q.begin(),Q.end(),comp); // y sortet pointlist


    for(unsigned index = 0; index < P.size(); ++index) {
        auto &p = P[index];
        auto &p1 = P[(index + 1) % P.size()];

        edges.emplace_back(p,p1);
    }


    //setup helper
    map<unsigned int,pointIndex> helper;

    for (unsigned int i = 1; i < Q.size();i++) {
        auto& vi = Q.at(i);
        auto& edge = edges.at(vi.index);

        auto Ay = edge.p1.pos.y;
        auto By = edge.p2.pos.y;
        auto& minEdgeY = std::min(Ay, By);
        auto& maxEdgeY = std::max(Ay, By);

        auto index = i-1;
        auto& b4 = Q.at(index);
        auto Y = b4.pos.y;
        while (Y > minEdgeY && Y < maxEdgeY){ // look if point is x parallel to the edge
            auto Bx = edge.p2.pos.x;
            auto Ax = edge.p1.pos.x;
            auto X = b4.pos.x;
            if(sign( (Bx - Ax)*(Y - Ay) - (By - Ay)*(X - Ax) ) < 0 // is the helper left to the edge direction
               && b4.vertexType != VertexType::SPLIT){
                helper.emplace(i,b4);
                return;
            }
            b4 = Q.at(index--);
            Y = b4.pos.y;
        }
    }




    vector<edge> T;

    for (auto& vi : Q){

        switch(vi.vertexType){
            case START:{
                auto& e = edges[vi.index];
                T.emplace_back(e);
                helper.at(vi.index) = vi.index;
            } break;
            case END:{
                auto& e = edges[vi.index];
                T.emplace_back(e);
                helper.at(vi.index) = vi.index;
            } break;
        }
    }


}

vector<triangleMesh::pointIndex> triangleMesh::triangluateMonoton(vector<pointIndex> MontonPolygon) {
    vector<pointIndex> Triangles;

    vector<pointIndex> V;
    vector<pointIndex> L;
    unsigned int count = -1;
    for(auto& p : MontonPolygon){
        V.push_back(p);
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

    return Triangles;
}
