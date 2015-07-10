#include <iostream>
#include <fstream>      // std::filebuf

#define PNG_DEBUG 3
#include <png.h>

#define PNGSIGSIZE 8

using namespace std;

bool validate(std::istream& source) {

    //Allocate a buffer of 8 bytes, where we can put the file signature.
    png_byte pngsig[PNGSIGSIZE];
    int is_png = 0;

    //Read the 8 bytes from the stream into the sig buffer.
    source.read((char*)pngsig, PNGSIGSIZE);

    //Check if the read worked...
    if (!source.good()) return false;

    //Let LibPNG check the sig. If this function returns 0, everything is OK.
    is_png = png_sig_cmp(pngsig, 0, PNGSIGSIZE);
    return (is_png == 0);
}

int main() {

    std::ifstream is;
    is.open("test.png",std::ifstream::in | ios_base::binary);
    std::cout << "png is valid? " << validate(is) << std::endl;
    is.close();

    return 0;
}