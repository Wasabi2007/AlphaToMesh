//
// Created by Jerry on 18.07.2015.
//

#include "../lodepng/lodepng.h"
#include "renderImage.h"
#include <string>
#include <gtx/string_cast.hpp>


renderImage::renderImage(const char* filename):renderImage(imageStruct::load(filename)){

}

renderImage::renderImage(imageStruct* img):
        img(img),VertexArrayID{0},vertexbuffer{0},
         elementbuffer{0}, VertexShader{0}, FragmentShader{0},
         ProgrammShader{0}, Texture{0},MVP{1}{
    initGeom();
    initShader();
    initTexture();
}

void renderImage::Render() {
    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

    glUseProgram(ProgrammShader);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void renderImage::initGeom() {
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);


    // An array of 3 vectors which represents 3 vertices
    const GLfloat g_vertex_buffer_data[] = {
            0.0f, 0.0f, 0.0f,//Position
            0.0f, 0.0f, //UV

            float(img->width), 0.0f, 0.0f,//Position
            1.0f, 0.0f, //UV

            float(img->width),  float(img->height), 0.0f,//Position
            1.0f, 1.0f, //UV

            0.0f,  float(img->height), 0.0f,//Position
            0.0f, 1.0f, //UV
    };


    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);

    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            (3*4)+8,                  // stride
            (void*)0            // array buffer offset
    );

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            (3*4)+8,                  // stride
            (void*)(3*4)            // array buffer offset
    );

    static std::vector<unsigned int> indices;
    indices.clear();
// fill "indices" as needed

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);


// Generate a buffer for the indices
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void renderImage::initShader() {
    // Create the shaders
    VertexShader = glCreateShader(GL_VERTEX_SHADER);
    FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    VertexShaderCode += "#version 330\n";
    VertexShaderCode += "in vec4 in_pos;\n";
    VertexShaderCode += "in vec2 in_uv;\n";
    VertexShaderCode += "uniform mat4 MVP;\n";
    VertexShaderCode += "out vec2 var_uv;\n";
    VertexShaderCode += "void main(){\n";
    VertexShaderCode += "gl_Position=MVP*in_pos;\n";
    VertexShaderCode += "var_uv=in_uv;\n";
    VertexShaderCode += "}\n";


    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    FragmentShaderCode += "#version 330\n";
    FragmentShaderCode += "in vec2 var_uv;\n";
    FragmentShaderCode += "uniform sampler2D tex;\n";
    FragmentShaderCode += "out vec4 out_color;\n";
    FragmentShaderCode += "void main(){\n";
    FragmentShaderCode += "out_color=texture(tex,var_uv);\n";
    FragmentShaderCode += "}\n";

    GLint Result = GL_FALSE;
    int InfoLogLength;

    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShader, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShader);

    // Check Vertex Shader
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShader, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Compile Fragment Shader
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShader, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShader);

    // Check Fragment Shader
    glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShader, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    // Link the program
    //fprintf(stdout, "Linking program\n");
    ProgrammShader = glCreateProgram();
    glAttachShader(ProgrammShader, VertexShader);
    glAttachShader(ProgrammShader, FragmentShader);


    glBindAttribLocation(ProgrammShader, 1, "in_pos");//Attribut Nummer 1 soll in in_pos im Vertex Shader zur Verf�gung stehen
    glBindAttribLocation(ProgrammShader, 2, "in_uv");//Attribut Nummer 3 soll in in_uv im Vertex Shader zur Verf�gung stehen
    glBindFragDataLocation(ProgrammShader, 0, "out_color");//out_color ist Farbe 0 (die in dem Framebuffer geschrieben werden)


    glLinkProgram(ProgrammShader);

    // Check the program
    glGetProgramiv(ProgrammShader, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgrammShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( InfoLogLength );
    glGetProgramInfoLog(ProgrammShader, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glUseProgram(ProgrammShader);
    glUniform1i(glGetUniformLocation(ProgrammShader, "tex"), 0);

    reloadMVP(mat4());
}

renderImage::~renderImage(){
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
    glDeleteShader(ProgrammShader);
}

void renderImage::reloadMVP(mat4 mvp){
    MVP = mvp;
    //std::cout << glm::to_string(MVP) << std::endl;

    glUseProgram(ProgrammShader);
    GLint MatrixID = glGetUniformLocation(ProgrammShader, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
}

void renderImage::initTexture() {
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);

    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
