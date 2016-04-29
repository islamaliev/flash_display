#include "../include/Stage.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "../include/Program.h"
#include <cmath>

using Stage = flash::display::Stage;
using Program = flash::render::Program;

namespace {
    GLuint vao;
    Program program;

    void update(double time) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         const GLfloat color[] = {(float)sin(time) * 0.5f + 0.5f, (float)cos(time) * 0.5f + 0.5f, 0.4f, 1.0f};
        const GLfloat color[] = {0.1, 0.1, 0.1, 1};
        glClearBufferfv(GL_COLOR, 0, color);

        program.activate(nullptr);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void dispose() {
        glDeleteVertexArrays(1, &vao);
        program.dispose();
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

        program.init();

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


