//
// Created by Jerry on 18.07.2015.
//

#ifndef ALPHATOMESH_IMAGESTRUCT_HPP
#define ALPHATOMESH_IMAGESTRUCT_HPP

#include "../glm/glm.hpp"
#include <vector>
#include <iostream>
#include "../lodepng/lodepng.h"

using namespace std;
using namespace glm;

struct imageStruct {
    vector<unsigned char> image; //the raw pixels
    vector<vec4> color; // color from 0.0 to 1.0
    const unsigned width, height;

    imageStruct(const vector<unsigned char> &image, unsigned int width, unsigned int height) : image(image),
                                                                                               width(width),
                                                                                               height(height) {
        for(auto y = 0u;y < height; ++y){
            for(auto x = 0u;x < width; ++x) {
                auto index = y * width * 4 + x * 4;

                auto r = float(image.at(index)) / 255.f;
                auto g = float(image.at(index + 1)) / 255.f;
                auto b = float(image.at(index + 2)) / 255.f;
                auto a = float(image.at(index + 3)) / 255.f;

                color.emplace_back(r, g, b, a);
            }
        }
    }


    static imageStruct* load(const char *filename) {
        std::vector<unsigned char> png;
        std::vector<unsigned char> image; //the raw pixels
        std::vector<unsigned char> bufimage;
        unsigned width, height;

        //load and decode
        lodepng::load_file(png, filename);
        unsigned error = lodepng::decode(bufimage, width, height, png);

        for(auto i = 0; i < width*height;++i){ // converting into opengl frendly pixel layout
            auto a = bufimage.back();
            bufimage.pop_back();
            auto b = bufimage.back();
            bufimage.pop_back();
            auto g = bufimage.back();
            bufimage.pop_back();
            auto r = bufimage.back();
            bufimage.pop_back();

            image.push_back(r);
            image.push_back(g);
            image.push_back(b);
            image.push_back(a);
        }

        //if there's an error, display it
        if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

        //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
        return new imageStruct(image, width, height);
    }

    vec4 getPixel(long x, long y) const {
        if(x < 0 || y < 0 || x >= width || y >= height){
            return vec4(0.f, 0.f, 0.f, 0.f); // retun invisible if we step over bounderays
        }
        auto index = y * width + x ;
        return color.at(index);
    }

    vec4 getPixel(ivec2 pos) const{
        return  getPixel(pos.x,pos.y);
    }

};

#endif //ALPHATOMESH_IMAGESTRUCT_HPP
