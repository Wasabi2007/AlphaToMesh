//
// Created by Jerry on 19.07.2015.
//

#ifndef ALPHATOMESH_SHADER_H
#define ALPHATOMESH_SHADER_H


#include "../glew/include/GL/glew.h"
#include <string>
#include <vector>

struct shader {
    GLuint VertexShader;
    GLuint FragmentShader;
    GLuint ProgrammShader;

    bool loaded;

    shader():VertexShader(0),FragmentShader(0),ProgrammShader(0),loaded(false){}

    shader(std::string VertexShaderCode, std::string FragmentShaderCode):VertexShader(0),FragmentShader(0),ProgrammShader(0),loaded(true){
        // Create the shaders
        VertexShader = glCreateShader(GL_VERTEX_SHADER);
        FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

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
        fprintf(stdout, "Linking program\n");
        ProgrammShader = glCreateProgram();
        glAttachShader(ProgrammShader, VertexShader);
        glAttachShader(ProgrammShader, FragmentShader);


        glBindAttribLocation(ProgrammShader, 1, "in_pos");//Attribut Nummer 1 soll in in_pos im Vertex Shader zur Verfügung stehen
        glBindFragDataLocation(ProgrammShader, 0, "out_color");//out_color ist Farbe 0 (die in dem Framebuffer geschrieben werden)


        glLinkProgram(ProgrammShader);

        // Check the program
        glGetProgramiv(ProgrammShader, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgrammShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::vector<char> ProgramErrorMessage( InfoLogLength );
        glGetProgramInfoLog(ProgrammShader, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
    }

    void loadShader() const {
        glUseProgram(ProgrammShader);
    }
};


#endif //ALPHATOMESH_SHADER_H
