//
// Created by jerry on 14.08.15.
//

#include "Rim.h"
//#include <lodepng.h>
#include "imageStruct.hpp"
#include <algorithm>

using namespace glm;
using namespace std;

Rim::Rim(float alpha_limit, const imageStruct *img, float errorMargin):alpha_limit{alpha_limit}
        ,img{img}
        ,errorMargin{errorMargin} {
    revaluate();
}

const std::vector<std::vector<glm::ivec2>>& Rim::findRims() {
    vector<vector<ivec2>> rims;
    vector<ivec2> rim;

    vector<vector<ivec2>> directions; //todo find a method to compute this
    directions.resize(8);

    directions.at(0).push_back(ivec2(1,0)); //walking directionm
    directions.at(0).push_back(ivec2(0,-1)); //where ist rigth ?

    directions.at(1).push_back(ivec2(1,1));
    directions.at(1).push_back(ivec2(0,-1));

    directions.at(2).push_back(ivec2(0,1));
    directions.at(2).push_back(ivec2(1,0));

    directions.at(3).push_back(ivec2(-1,1));
    directions.at(3).push_back(ivec2(1,0));

    directions.at(4).push_back(ivec2(-1,0));
    directions.at(4).push_back(ivec2(0,1));

    directions.at(5).push_back(ivec2(-1,-1));
    directions.at(5).push_back(ivec2(0,1));

    directions.at(6).push_back(ivec2(0,-1));
    directions.at(6).push_back(ivec2(-1,0));

    directions.at(7).push_back(ivec2(1,-1));
    directions.at(7).push_back(ivec2(-1,0));

    vector<vector<bool> > partOfChain; // save witch pixels are already part of a chain. Cheaper then looking throug all existing rims.
    partOfChain.resize(img->height+2); // +2 to compensate the grap over the imageborder
    for (int i = 0; i < img->height+2; ++i)
        partOfChain[i].resize(img->width+2);

    //walk over the while image
    for (long y = -1; y <= long(img->height); ++y) {
        for (long x = -1; x <= long(img->width); ++x) {
            if(partOfChain[y+1][x+1]) continue; // skip pixel that is already in a chain

            auto pixel = img->getPixel(x,y); //current position
            auto next = img->getPixel(x+1,y); //future pixel

            auto start = ivec2(static_cast<int>(x),static_cast<int>(y));

            //find every possible alpha rim
            if(pixel.a <= alpha_limit && next.a > alpha_limit) { // check if we are on the rim of an "object" and is part of a rim
                unsigned long long dir = 0;
                //cout << endl << "new Rim" << endl;
                //cout<< "("<< start.x << "/"<< start.y << ")";
                int count = 0;

                bool pixelOnly = false;

                //get a head start
                while (img->getPixel(start + directions.at(dir).at(0)).a >
                       alpha_limit // is where I want to walk still enough transparancy?
                       || img->getPixel(start + directions.at(dir).at(0) + directions.at(dir).at(1)).a <=
                          alpha_limit) { // is rigth from that pixel still an opace pixel?
                    dir = (dir + 1) % directions.size(); // if we didn't found it look into an other direction
                    count++;
                    if (count > 7) { // only one pixel transparent
                        pixelOnly = true;
                        break;
                    }
                    assert(count < 8 && "start"); // if we looked into all directions and didn't found anything
                }
                if (!pixelOnly) {

                    auto current =
                            start + directions.at(dir).at(0); // set headstart so that the search dont end imdiataly;
                    rim.push_back(start);
                    partOfChain[start.y + 1][start.x + 1] = true;
                    partOfChain[current.y + 1][current.x + 1] = true;
                    auto pixelcount = 0;
                    //todo maybe change to a do while
                    while (current != start) { //Run along the rim till we have a loop

                        //prevent a loop that doesn't end in the start
                        auto found = find(rim.begin(), rim.end(), current);
                        if (found != rim.end()) {

                            for (auto &p : rim) {
                                partOfChain[p.y + 1][p.x + 1] = false;
                            }

                            vector<ivec2> buf;
                            for (; found != rim.end(); found++) {
                                buf.push_back(*found);
                                partOfChain[(*found).y + 1][(*found).x + 1] = true;
                            }
                            rim.swap(buf);
                            break;
                        }


                        count = 0;
                        //find next rim pixel
                        auto maindir = current + directions.at(dir).at(0);
                        auto maindirNormal = current + directions.at(dir).at(0) + directions.at(dir).at(1);

                        auto dirFound = true;

                        while (img->getPixel(maindir).a > alpha_limit
                               || img->getPixel(maindirNormal).a <= alpha_limit
                               || partOfChain[maindir.y + 1][maindir.x + 1]) { // first run ignoring chain parts
                            dir = (dir + 1) % directions.size();

                            maindir = current + directions.at(dir).at(0);
                            maindirNormal = current + directions.at(dir).at(0) + directions.at(dir).at(1);

                            count++;
                            if (count > 8) {
                                dirFound = false;
                                break;
                            }

                            //assert(count < 8 && "chain");
                        }

                        if (!dirFound) { // now with already pixel that are already part of a chain
                            count = 0;
                            while (img->getPixel(maindir).a > alpha_limit
                                   || img->getPixel(maindirNormal).a <= alpha_limit) {
                                dir = (dir + 1) % directions.size();

                                maindir = current + directions.at(dir).at(0);
                                maindirNormal = current + directions.at(dir).at(0) + directions.at(dir).at(1);

                                count++;
                                assert(count < 8 && "chain");
                            }
                        }

                        //cout<< "->("<< current.x << "/"<< current.y << ")";

                        //add found chainlink into rim
                        rim.push_back(current);
                        partOfChain[current.y + 1][current.x + 1] = true;
                        current = current + directions.at(dir).at(0);
                        pixelcount++;
                        assert(pixelcount < img->width *
                                            img->height); // we somehow ended in a loop that is longer than we have pixels
                    }

                    rims.push_back(rim); // add rim to found rims
                    rim = vector<ivec2>(); // an new rim for the next run
                }
            }
        }
    }
    fullRims.swap(rims);
    return fullRims;
}

std::vector<glm::vec2> Rim::simplyfiyRim(const std::vector<glm::vec2>& rim) {
    //DouglasPeucker
    // Find the point with the maximum distance

    vector<vec2> erg;

    auto dmax = 0.f;
    auto index = size_t(0);
    auto end = rim.size()-1;
    for(auto i = size_t(1); i < ( end ); ++i) {
        auto line = rim[end]-rim[0];
        auto relpoint = rim[i]-rim[0];
        auto distToLine = (dot(relpoint,line)/dot(line,line)*line)-relpoint;
        auto d = length(distToLine);
        if ( d > dmax ) {
            index = i;
            dmax = d;
        }
    }
    // If max distance is greater than epsilon, recursively simplify
    if ( dmax > errorMargin ) {
        // Recursive call
        auto recResults1 = simplyfiyRim(vector<vec2>(rim.begin(),rim.begin()+index));
        auto recResults2 = simplyfiyRim(vector<vec2>(rim.begin()+index,rim.end()));

        // Build the result list
        erg = recResults1;//{recResults1[1...length(recResults1)-1] recResults2[1...length(recResults2)]}
        erg.insert(erg.end(),recResults2.begin(),recResults2.end());
    } else {
        erg = vector<vec2>();
        erg.push_back(rim[0]);
        erg.push_back(rim[end]);
    }
    // Return the result

    return erg;
}

std::vector<glm::vec2> Rim::simplyfiyRim(const std::vector<glm::ivec2>& rim) {
    vector<vec2> erg;
    for (auto &r : rim) {
        erg.emplace_back(float(r.x), float(r.y));
    }
    return simplyfiyRim(erg);
}

const std::vector<std::vector<glm::vec2>>& Rim::simplyfiyRims() {
    vector<vector<vec2>> erg;
    for(auto& rim : fullRims){
        if(rim.size() > 2) {
            erg.push_back(simplyfiyRim(rim));
        }
    }
    simpleRims.swap(erg);
    return simpleRims;
}

void Rim::revaluate() {
    findRims();
    simplyfiyRims();
}
