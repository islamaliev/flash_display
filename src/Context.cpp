#include "Contex.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "Program.h"
#include "DisplayObject.h"
#include <iostream>
#include <vector>
#include <cassert>
#include "StackAllocator.h"
#include "RenderState.h"
#include "Texture.h"

using Program = flash::render::Program;
using Texture = flash::display::Texture;
using Mat4 = flash::math::Mat4;
using StackAllocator = flash::allocator::StackAllocator;
using Marker = StackAllocator::Marker;

#define MAX_TREE_DEPTH 50

namespace {
    Program program;
    GLFWwindow* window;

    std::vector<unsigned> _textures;
    StackAllocator _frameAllocator = StackAllocator(10000);
    std::vector<float> _useTextures;

    void _init() {
        _textures = {};
        _textures.reserve(10);

        _useTextures = {};
        _useTextures.reserve(1000);
    }

    void _clear() {
        _frameAllocator.clear();
        _textures.clear();
        _useTextures.clear();
    }
}

namespace {
    GLuint _vao = 0;

    float _points[] = {
            0.0f,  1.0f,  0.0f,
            1.0f, 1.0f,  0.0f,
            1.0f, 0.0f,  0.0f,
            0.0f, 0.0f,  0.0f
    };

    GLuint _indecies[] = {
            0, 1, 2,
            3, 0, 2
    };

    int _firstTexture = -1;

    void _initVAO() {
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        GLuint indicesBuffer = 0;
        glGenBuffers(1, &indicesBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indecies), _indecies, GL_STATIC_DRAW);
    }
}

#ifdef OFFSCREEN
#include <assert.h>
#include <png.h>
#include <jpeglib.h>

extern const char* nextOffscreen();

namespace {

    GLuint fbo = 0;
    GLuint rbo_depth = 0;
    GLuint renderTexture = 0;

    unsigned w;
    unsigned h;

    enum Constants { SCREENSHOT_MAX_FILENAME = 256 };

    void prepareOffscreenBuffer() {
        glGenTextures(1, &renderTexture);
        glBindTexture(GL_TEXTURE_2D, renderTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

        glGenRenderbuffers(1, &rbo_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture, 0);

        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

        int glget;
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &glget);
        assert(w * h < (unsigned int) glget);
    }

    void savePNG(const char *filename) {
        size_t i, nvals;
        const size_t format_nchannels = 3;
        nvals = format_nchannels * w * h;
        FILE *f = fopen(filename, "wb");

        if (!f) {
            printf("Error opening output png file %s\n", filename );
            return;
        }

        GLubyte* pixels = (GLubyte*) malloc(nvals * sizeof(GLubyte));
        png_byte* png_bytes = (png_byte*) malloc(nvals * sizeof(png_byte));
        png_byte** png_rows = (png_byte**) malloc(h * sizeof(png_byte*));
        glBindTexture(GL_TEXTURE_2D, renderTexture);
        glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        for (i = 0; i < nvals; i++)
            (png_bytes)[i] = (pixels)[i];
        for (i = 0; i < h; i++)
            (png_rows)[h - i - 1] = &(png_bytes)[i * w * format_nchannels];
        png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png) abort();
        png_infop info = png_create_info_struct(png);
        if (!info) abort();
        if (setjmp(png_jmpbuf(png))) abort();
        png_init_io(png, f);
        png_set_IHDR(
                png,
                info,
                w,
                h,
                8,
                PNG_COLOR_TYPE_RGB,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT
        );
        png_write_info(png, info);
        png_write_image(png, png_rows);
        png_write_end(png, NULL);
        free(pixels);
        free(png_bytes);
        free(png_rows);
        fclose(f);
    }

    void saveOffscreen(const char* name) {
        glFlush();
        char filename[SCREENSHOT_MAX_FILENAME];
        strcpy(filename, name);
        strcat(filename, ".png");
        savePNG(filename);
    }
}
#endif


using namespace flash;
using namespace render;

void Context::init(unsigned width, unsigned height) {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    assert(glewInit() == GLEW_OK);

#ifdef OFFSCREEN
    w = width;
    h = height;
    glfwHideWindow(window);
    prepareOffscreenBuffer();
#else
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);
    glReadBuffer(GL_BACK);
#endif

    _init();
    _initVAO();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    program.init();
    program.activate(nullptr);
}

void Context::start(flash::display::DisplayObject& stage) {
    glClearColor(0.1, 0.1, 0.1, 1);
    glClearDepth(1.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef OFFSCREEN
        glViewport(0, 0, w, h);
        const char* name = nextOffscreen();
        if (strlen(name) == 0) {
            continue;
        }
#endif

        _clear();

        RenderState renderState;
        stage.draw(*this, renderState);

        ComponentContainer& components = stage._getComponents();
        components.sort();

        Mat4* parentMatrices = (Mat4*) _frameAllocator.alloc(sizeof(Mat4) * MAX_TREE_DEPTH);
        *parentMatrices = Mat4();

        Marker matricesMarker = _frameAllocator.getMarker();

        components.forEach([parentMatrices](SpatialComponent& spatial, int depth) {
            if (!depth)
                return;
            Mat4* m = (Mat4*) _frameAllocator.alloc(sizeof(Mat4));
            *m = Mat4();
            float xt = spatial.x - spatial.pivotX * spatial.scaleX;
            float yt = spatial.y - spatial.pivotY * spatial.scaleY;
            m->translate(xt, yt, 0);
            m->scale(spatial.width, spatial.height, 0);
            *m = parentMatrices[depth] = parentMatrices[depth - 1] * *m;
        });

        auto matricesSize = _frameAllocator.getMarker() - matricesMarker;

        glBindVertexArray(_vao);
        if (_textures.size() > 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _textures[0]);
            program.setUniform("u_texture0", 0);
            if (_textures.size() > 1) {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, _textures[1]);
                program.setUniform("u_texture1", 1);
            }
        }

        glBindVertexArray(_vao);

        auto pointsSize = sizeof(_points);
        auto useTexturesSize = sizeof(float) * _useTextures.size();

        GLuint vertexBuffer = 0;
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, pointsSize + useTexturesSize + matricesSize, NULL, GL_STATIC_DRAW);

        unsigned offset = 0;
        glBufferSubData(GL_ARRAY_BUFFER, offset, pointsSize, _points);
        offset += pointsSize;
        glBufferSubData(GL_ARRAY_BUFFER, offset, useTexturesSize, &_useTextures[0]);
        offset += useTexturesSize;
        glBufferSubData(GL_ARRAY_BUFFER, offset,  matricesSize, _frameAllocator.getPointer(matricesMarker));

        auto matRowSize = 4 * sizeof(float);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void*) pointsSize);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * matRowSize, (void*) (pointsSize + useTexturesSize + 0 * matRowSize));
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * matRowSize, (void*) (pointsSize + useTexturesSize + 1 * matRowSize));
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * matRowSize, (void*) (pointsSize + useTexturesSize + 2 * matRowSize));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matRowSize, (void*) (pointsSize + useTexturesSize + 3 * matRowSize));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);

        program.activate(nullptr);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, (GLsizei) (matricesSize / sizeof(Mat4)));

        glfwPollEvents();

#ifdef OFFSCREEN
        saveOffscreen(name);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#else
        glfwSwapBuffers(window);
#endif
    }
    dispose();
}

void Context::stop() {
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void Context::dispose() {
    glDeleteVertexArrays(1, &_vao);
    program.dispose();
    glfwTerminate();
}

void Context::setProjection(const flash::math::Mat4& matrix) {
    program.setUniform("u_projection", matrix);
}

// TODO: make Texture const?
void Context::setTexture(Texture* texture) {
    if (!texture->getId()) {
        texture->bindData();
        if (_firstTexture == -1) {
            _firstTexture = texture->getId();
        }
        _textures.push_back(texture->getId());
    }
    _useTextures.push_back(_firstTexture == texture->getId() ? 0 : 1);
}

void Context::unsetTexture() {
    _useTextures.push_back(-1);
}