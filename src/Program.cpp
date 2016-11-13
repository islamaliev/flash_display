#import <OpenGL/OpenGL.h>
#import <OpenGL/gl3.h>
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
            char *ProgramErrorMessage = new char[InfoLogLength+1];
            glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ProgramErrorMessage);
            printf("%s\n", &ProgramErrorMessage[0]);
            delete[] ProgramErrorMessage;
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
            char *VertexShaderErrorMessage = new char[InfoLogLength+1];
            glGetShaderInfoLog(shaderID, InfoLogLength, NULL, VertexShaderErrorMessage);
            printf("%s\n", &VertexShaderErrorMessage[0]);
            delete[] VertexShaderErrorMessage;
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

        layout (location = 0) in vec3 position;
        layout (location = 1) in int u_useTexture;
        layout (location = 2) in mat4 transform;

        uniform mat4 u_projection;

        out Fragment {
            vec2 tc;
            flat int u_textureIndex;
        } fragment;

        void main() {
            fragment.tc.x = position.x;
            fragment.tc.y = 1 - position.y;
            fragment.u_textureIndex = u_useTexture;
            gl_Position = u_projection * transform * vec4(position, 1.0);
            
            /*fragment.tc.x = position.x;
            fragment.tc.y = 1 - position.y;
            fragment.u_textureIndex = u_useTexture;
			int x = gl_VertexID % 2;
			int y = gl_VertexID / 2;
			
			int xOffset = gl_InstanceID % 4;
			int yOffset = gl_InstanceID / 4;
			xOffset *= 10;
			yOffset *= 10;

			mat4 m;
			m[0] = vec4(5, 0, 0, 0);
			m[1] = vec4(0, 5, 0, 0);
			m[2] = vec4(0, 0, 1, 0);
			m[3] = vec4(x + xOffset, y + yOffset, 0, 1);
			gl_Position = u_projection * m * vec4(position, 1.0);*/
        })shaderCode";

    const std::string& fragShaderCode = R"shaderCode(
        #version 330 core

        uniform sampler2D u_textures[16];
        out vec4 color;

        in Fragment {
            vec2 tc;
            flat int u_textureIndex;
        } fragment;

        void main() {
            if (fragment.u_textureIndex == -1) {
                color = vec4(1, 0, 0, 1);
            } else {
                color = texture(u_textures[fragment.u_textureIndex], fragment.tc);
            }
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
    const GLint location = glGetUniformLocation(program, name);
    glUniform1i(location, val);
}

void Program::setUniform(const char* name, int* val, int count) {
    const GLint location = glGetUniformLocation(program, name);
    glUniform1iv(location, count, val);
}