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

    /*const std::string& vertShaderCode = R"shaderCode(
        #version 330 core

        layout (location = 0) in vec3 position;
//        layout (location = 1) in int u_useTexture;
//        layout (location = 2) in vec4 C0;
//        layout (location = 3) in vec4 C1;
//        layout (location = 4) in vec4 C2;
//        layout (location = 5) in vec4 C3;
//        in sampler2D u_texture;

        uniform int u_useTexture;
        uniform mat4 u_matrix;
        uniform mat4 u_projection;

        out VS_OUT {
            vec2 tc;
//            sampler2D u_texture;
            flat int u_textureIndex;
        } vs_out;

        void main() {
//            vs_out.u_texture = u_texture;
            vs_out.u_textureIndex = u_useTexture;
            vs_out.tc.x = position.x;
            vs_out.tc.y = 1 - position.y;
//            gl_Position = u_projection * mat4(C0, C1, C2, C3) * vec4(position, 1.0);
            gl_Position = u_projection * u_matrix * vec4(position, 1.0);
        })shaderCode";

    const std::string& fragShaderCode = R"shaderCode(
        #version 330 core

        uniform sampler2D u_texture0;
        uniform sampler2D u_texture1;
        //uniform sampler2D u_texture;
        //uniform bool u_useTexture;

        out vec4 color;

        in VS_OUT {
            vec2 tc;
//            sampler2D u_texture;
            flat int u_textureIndex;
        } fs_in;

        void main() {
//            if (fs_in.u_textureIndex == 1) {
//                color = texture(u_texture0, fs_in.tc);
//            } else if (fs_in.u_textureIndex == 2) {
//                color = texture(u_texture1, fs_in.tc);
//            } else {
                color = vec4(1, 0, 0, 1);
//            }
        })shaderCode";*/

    const std::string& vertShaderCode = R"shaderCode(
        #version 330 core
        in vec3 position;
        uniform mat4 u_matrix;
        uniform mat4 u_projection;
        out VS_OUT {
            vec2 tc;
        } vs_out;
        void main() {
            vs_out.tc.x = position.x;
            vs_out.tc.y = 1 - position.y;
            gl_Position = u_projection * u_matrix * vec4(position, 1.0);
        })shaderCode";

    const std::string& fragShaderCode = R"shaderCode(
        #version 330 core
        uniform sampler2D u_texture;
        uniform bool u_useTexture;
        out vec4 color;
        in VS_OUT {
            vec2 tc;
        } fs_in;
        void main()
        {
            if (u_useTexture) {
                color = texture(u_texture, fs_in.tc);
            } else {
                color = vec4(1, 0, 0, 1);
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