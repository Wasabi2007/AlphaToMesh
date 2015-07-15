#include <iostream>
#include <fstream>      // std::filebuf
#include <vector>
#include <iomanip>

//#define PNG_DEBUG 3
//#include <lodepng/lodepng.h>
#include "glm/glm.hpp"
#include "lodepng/lodepng.h"

#define PNGSIGSIZE 8

using namespace std;
using namespace glm;

struct imageStruct {
    std::vector<unsigned char> image; //the raw pixels
    const unsigned width, height;

    imageStruct(const vector<unsigned char> &image, unsigned int width, unsigned int height) : image(image),
                                                                                               width(width),
                                                                                               height(height) { }

    vec4 getPixel(unsigned x, unsigned y) {
        auto index = y * width * 4 + x * 4;

        auto r = float(image.at(index)) / 255.f;
        auto g = float(image.at(index + 1)) / 255.f;
        auto b = float(image.at(index + 2)) / 255.f;
        auto a = float(image.at(index + 3)) / 255.f;

        return vec4(r, g, b, a);
    }

};

imageStruct decode(const char *filename) {
    std::vector<unsigned char> png;
    std::vector<unsigned char> image; //the raw pixels
    unsigned width, height;

    //load and decode
    lodepng::load_file(png, filename);
    unsigned error = lodepng::decode(image, width, height, png);

    //if there's an error, display it
    if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
    return imageStruct(image, width, height);
}


int main() {
    //std::ifstream is;
    //is.open("test.png",std::ifstream::in | ios_base::binary);
    //std::cout << "png is valid? " << validate(is) << std::endl;
    //is.close();

    auto img = decode("test.png");


    std::cout << std::fixed << std::setprecision(1);
    for (unsigned y = 0; y < img.height; ++y) {
        for (unsigned x = 0; x < img.width; ++x) {
            std::cout << (img.getPixel(x, y).a>0.5?"#":(img.getPixel(x, y).a>0.25?"+":(img.getPixel(x, y).a>0.01?"-":" ")));
        }
        std::cout << std::endl;
    }

    return 0;
}