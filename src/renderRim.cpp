//
// Created by Jerry on 18.07.2015.
//


#include "renderRim.h"
#include <string>



renderRim::renderRim(const std::vector<glm::ivec2>& posin, long width, long height):
        VertexArrayID{0},vertexbuffer{0},
        VertexShader{0}, FragmentShader{0},
        ProgrammShader{0}, Texture{0}{

    for(auto& p : posin) {
        pos.emplace_back((float(p.x)-width*0.5f)/float(width)*2, (float(p.y)-height*0.5f)/float(height)*2,0.0f);
    }

    initGeom();
    initShader();
}

renderRim::renderRim(const std::vector<glm::vec2>& posin, long width, long height):
        VertexArrayID{0},vertexbuffer{0},
        VertexShader{0}, FragmentShader{0},
         ProgrammShader{0}, Texture{0}{

    for(auto& p : posin) {
        pos.emplace_back((p.x-width*0.5f)/width*2, (p.y-height*0.5f)/height*2,0.0f);
    }

    initGeom();
    initShader();
}

void renderRim::Render() {
    glBindVertexArray(VertexArrayID);
    glUseProgram(ProgrammShader);
    glDrawArrays(GL_LINE_LOOP,0, GLsizei( pos.size()));
}

void renderRim::initGeom() {
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);

    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*pos.size(), pos.data(), GL_STATIC_DRAW);

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

}

void renderRim::initShader() {
    // Create the shaders
    VertexShader = glCreateShader(GL_VERTEX_SHADER);
    FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    VertexShaderCode += "#version 420\n";
    VertexShaderCode += "in vec4 in_pos;\n";
    VertexShaderCode += "void main(){\n";
    VertexShaderCode += "gl_Position=in_pos;\n";
    VertexShaderCode += "}\n";


    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    FragmentShaderCode += "#version 420\n";
    FragmentShaderCode += "out vec4 out_color;\n";
    FragmentShaderCode += "void main(){\n";
    FragmentShaderCode += "out_color=vec4(0,1,0,1);\n";
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

