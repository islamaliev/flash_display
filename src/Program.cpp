#include <GL/glew.h>
#include <cstdio>
#include "../include/Program.h"
#include "../include/Contex.h"
#include <string>

namespace {
    GLuint vertShader;
    GLuint fragShader;
    GLuint program;

    GLint Result;
    int InfoLogLength;

    void _cleanUp() {
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
    }

    void _checkProgram(GLuint ProgramID) {
        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ) {
            char ProgramErrorMessage[InfoLogLength+1];
            glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }
    }

    void _linkProgram(GLuint ProgramID) {
        glAttachShader(ProgramID, vertShader);
        glAttachShader(ProgramID, fragShader);
        glLinkProgram(ProgramID);
    }

    void _checkShader(GLuint shaderID) {
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0 ) {
            char VertexShaderErrorMessage[InfoLogLength+1];
            glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            printf("%s\n", &VertexShaderErrorMessage[0]);
        }
    }

    void _compileShader(const std::string& shaderCode, const GLuint& shaderID) {
        char const *sourcePointer = shaderCode.c_str();
        glShaderSource(shaderID, 1, &sourcePointer, NULL);
        glCompileShader(shaderID);
    }

    void _getShader(GLuint shaderID, const std::string shaderCode) {
        _compileShader(shaderCode, shaderID);
        _checkShader(shaderID);
    }

}

using namespace flash;
using namespace render;

Program::Program() {

}

void Program::init() {
    Result = GL_FALSE;

    const std::string& vertShaderCode = R"shaderCode(
        #version 330 core

        in vec3 vp;

        void main(){
            const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, 0.5, 1.0),
                                             vec4(-0.25, -0.25, 0.5, 1.0),
                                             vec4(0.25, 0.25, 0.5, 1.0));
            gl_Position = vertices[gl_VertexID];
        })shaderCode";

    const std::string& fragShaderCode = R"shaderCode(
        #version 330 core

        out vec3 color;

        void main()
        {
            color = vec3(1,0,0);
        })shaderCode";

    vertShader = glCreateShader(GL_VERTEX_SHADER);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    _getShader(vertShader, vertShaderCode);
    _getShader(fragShader, fragShaderCode);

    program = glCreateProgram();
    _linkProgram(program);
    _checkProgram(program);
    _cleanUp();
}

void Program::activate(Context* context) {
    glUseProgram(program);
}

void Program::dispose() {
    glDeleteProgram(program);
}
