#include "../include/Stage.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <cmath>

using Stage = flash::display::Stage;

namespace {
    GLuint vertShader;
    GLuint fragShader;
    GLuint program;
    GLuint vao;

    GLint Result;
    int InfoLogLength;

    void _cleanUp() {
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
    }

    void _checkProgram(GLuint ProgramID) {
        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
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
        if (InfoLogLength > 0 ){
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

    GLuint _getProgram() {
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

        GLuint ProgramID = glCreateProgram();
        _linkProgram(ProgramID);
        _checkProgram(ProgramID);
        _cleanUp();
        return ProgramID;
    }

    void update(double time) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         const GLfloat color[] = {(float)sin(time) * 0.5f + 0.5f, (float)cos(time) * 0.5f + 0.5f, 0.4f, 1.0f};
        const GLfloat color[] = {0.1, 0.1, 0.1, 1};
        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(program);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void dispose() {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
    }

    int init() {
        if (!glfwInit()) {
            fprintf(stderr, "ERROR: could not start GLFW3\n");
            return 1;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL", NULL, NULL);
        if (!window) {
            fprintf(stderr, "ERROR: could not open window with GLFW3\n");
            glfwTerminate();
            return 1;
        }
        glfwMakeContextCurrent (window);

        glewExperimental = GL_TRUE;
        glewInit ();

        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* version = glGetString(GL_VERSION);
        printf("Renderer: %s\n", renderer);
        printf("OpenGL version supported %s\n", version);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // INIT
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        program = _getProgram();

        // LOOP
        while (!glfwWindowShouldClose(window)) {
            update((double) clock() / 10000);

            glfwPollEvents();
            glfwSwapBuffers(window);
        }

        // DISPOSE
        dispose();
        glfwTerminate();

        return 0;
    }
}

void Stage::start() {
    init();
}


