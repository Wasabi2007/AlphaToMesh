//
// Created by jerry on 14.08.15.
//

#ifndef ALPHATOMESH_RIM_H
#define ALPHATOMESH_RIM_H

#include <vector>
#include <glm.hpp>

class imageStruct;

class Rim {
private:
    const imageStruct* img;
    std::vector<std::vector<glm::ivec2>> fullRims;
    std::vector<std::vector<glm::vec2>> simpleRims;
    float alpha_limit;
    float errorMargin;

public:
    Rim(float alpha_limit, const imageStruct* img, float errorMargin);

    void revaluate();
    const std::vector<std::vector<glm::ivec2>>& findRims();
    const std::vector<std::vector<glm::vec2>>& simplyfiyRims();

    const std::vector<std::vector<glm::ivec2>>& getFullRims() const { return fullRims;}
    const std::vector<std::vector<glm::vec2>>& getSimpelRims() const { return simpleRims;}

private:
    //std::vector<std::vector<glm::ivec2>>  findRims() ;
    std::vector<glm::vec2> simplyfiyRim(const std::vector<glm::vec2>& rim) ;
    std::vector<glm::vec2> simplyfiyRim(const std::vector<glm::ivec2>& rim);
};


#endif //ALPHATOMESH_RIM_H
