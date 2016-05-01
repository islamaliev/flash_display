#include <GL/glew.h>
#include "DisplayObject.h"
#include "Contex.h"
#include "Mat4.h"

using namespace flash::display;
using namespace flash::render;

namespace {
    const unsigned FLOAT_PRO_POINT = 3;
//    const unsigned POINTS_NUM = 6;

    GLuint _vao;

    bool initialized = false;

    void initVAO(GLuint& vao) {
        float points[] = {
                -1.0f,  1.0f,  0.0f,
                1.0f, 1.0f,  0.0f,
                1.0f, -1.0f,  0.0f,
                -1.0f, -1.0f,  0.0f
        };

        GLint indecies[] = {
                0, 1, 2,
                3, 0, 2
        };

        GLuint vertexBuffer = 0;
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

        vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, FLOAT_PRO_POINT, GL_FLOAT, GL_FALSE, 0, NULL);

        GLuint indicesBuffer = 0;
        glGenBuffers(1, &indicesBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indecies), indecies, GL_STATIC_DRAW);
    }
}

DisplayObject::DisplayObject() {

}

void DisplayObject::draw(Context& context) {
    if (!initialized) {
        initVAO(_vao );
        initialized = true;
    }

    flash::math::Mat4 m;
    float xt = (x() + width() / 2) / 400 - 1;
    float yt = (y() + height() / 2) / 300 - 1;
    m.translate(xt, yt, 0);
    m.scale(width() / 800, height() / 600, 0);
    context.setMatrix(m);

    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}