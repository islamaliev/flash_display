#include <GL/glew.h>
#include "../include/DisplayObject.h"
#include "../include/Contex.h"

using namespace flash::display;
using namespace flash::render;

namespace {
    const unsigned FLOAT_PRO_POINT = 3;
    const unsigned POINTS_NUM = 6;

    GLuint _vao;

    bool initialized = false;

    void initVAO(GLuint& vao, const float* points, const unsigned size) {
        GLuint vbo = 0;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), points, GL_STATIC_DRAW);

        vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, FLOAT_PRO_POINT, GL_FLOAT, GL_FALSE, 0, NULL);
    }
}

DisplayObject::DisplayObject() {

}

void DisplayObject::draw(Context& context) {
    if (!initialized) {
        float points[] = {
                -0.5f,  0.5f,  0.0f,
                0.5f, 0.5f,  0.0f,
                0.5f, -0.5f,  0.0f,
                -0.5f, -0.5f,  0.0f,
                -0.5f,  0.5f,  0.0f,
                0.5f, -0.5f,  0.0f
        };

        initVAO(_vao, points, POINTS_NUM * FLOAT_PRO_POINT);
        initialized = true;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLenum mode;
//	mode = GL_POINTS;
//	mode = GL_LINE_STRIP;
    mode = GL_TRIANGLES;
//	mode = GL_LINES;

//    glUseProgram(shaderProgram);
    glBindVertexArray(_vao);
    glDrawArrays(mode, 0, POINTS_NUM);
}