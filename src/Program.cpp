#include <GL/glew.h>
#include <cstdio>
#include "Program.h"
#include "Contex.h"
#include <string>
#include <Mat4.h>

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

        in vec3 position;

        uniform mat4 matrix;
        uniform mat4 projection;

        out VS_OUT {
            vec2 tc;
        } vs_out;

        void main() {
            vs_out.tc.x = position.x;
            vs_out.tc.y = 1 - position.y;
            gl_Position = projection * matrix * vec4(position, 1.0);
        })shaderCode";

    const std::string& fragShaderCode = R"shaderCode(
        #version 330 core

        uniform sampler2D s;

        out vec4 color;

        in VS_OUT {
            vec2 tc;
        } fs_in;

        void main()
        {
            color = texture(s, fs_in.tc);
//            color = vec4(fs_in.tc.x, fs_in.tc.y, 0, 1);
            //color = texelFetch(s, ivec2(gl_FragCoord.xy), 0);
            //color = vec4(1, 0, 0, 1);
            //vec4 c = texture(s, vec2(0.1, 0.1));
            //color = vec4(c.rgb, 1.0);
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

void Program::setUniform(const char* name, const flash::math::Mat4& matrix) {
    const GLint matrixLocation = glGetUniformLocation(program, name);
    glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, matrix);
}

void Program::setUniform(const char* name, int val) {
    const GLint matrixLocation = glGetUniformLocation(program, name);
    glUniform1i(matrixLocation, val);
}