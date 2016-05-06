#include "Contex.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "Program.h"
#include "DisplayObject.h"
#include <iostream>
#include <RenderState.h>

using Program = flash::render::Program;

namespace {
    GLuint _vao;
    Program program;
    GLFWwindow* window;
}

using namespace flash;
using namespace render;

void Context::init(unsigned width, unsigned height) {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
//        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
//        return 1;
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

    program.init();
    program.activate(nullptr);
}

void Context::start(flash::display::DisplayObject& displayObject) {
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        double time = (double) clock() / 10000;
//        const GLfloat color[] = {(float)sin(time) * 0.5f + 0.5f, (float)cos(time) * 0.5f + 0.5f, 0.4f, 1.0f};
        const GLfloat color[] = {0.1, 0.1, 0.1, 1};
        glClearBufferfv(GL_COLOR, 0, color);

        RenderState renderState;
        displayObject.draw(*this, renderState);

//        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

}

void Context::dispose() {
    glDeleteVertexArrays(1, &_vao);
    program.dispose();
    glfwTerminate();
}

void Context::setMatrix(const flash::math::Mat4& matrix) {
    program.setUniform("matrix", matrix);
}

void Context::setProjection(const flash::math::Mat4& matrix) {
    program.setUniform("projection", matrix);
}