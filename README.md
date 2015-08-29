# Alpha to Mesh
A Program to generate a Mesh from the Alpha channel from a png.  
Bear in mind, this is more of a Prof of Concept than a usable Tools.

## Installation
This Program is Tested with TDM-GCC-5.1, linux gcc, linux clang.

### Windows Linux
You need Cmake on both platforms.

### Windows
There shouldn't be any extra dependencies.

### Linux
You need xorg-dev and libglu1-mesa-dev.

## Static linked library's
glew  
glfw  
glm  
lodepng  
nanogui  

## Usage
After staring the program you click "Open" and load your png.  
Than you fiddle with alpha limit and simplification to your preferences.  
Press "Find Rims" to for find the Rims.  
Now you can Click on "Play" or "Step" and watch the beautiful Triangulation Algorithm work.  
Finally after the Triangulation has finished, you can save the Mesh to an obj file.  

## License


The MIT License (MIT)  
  
Copyright (c) 2015 Jeremias Boos  
  
Permission is hereby granted, free of charge, to any person obtaining a copy  
of this software and associated documentation files (the "Software"), to deal  
in the Software without restriction, including without limitation the rights  
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell  
copies of the Software, and to permit persons to whom the Software is  
furnished to do so, subject to the following conditions:  
  
The above copyright notice and this permission notice shall be included in  
all copies or substantial portions of the Software.  
  
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN  
THE SOFTWARE.  