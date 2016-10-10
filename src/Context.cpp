#include "Contex.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <cassert>
#include "RenderBufferOrganizer.h"
#include "Program.h"
#include "DisplayObject.h"
#include "StackAllocator.h"
#include "RenderState.h"

using Mat4 = flash::math::Mat4;
using StackAllocator = flash::allocator::StackAllocator;
using Marker = StackAllocator::Marker;
using DisplayObject = flash::display::DisplayObject;

using namespace flash;
using namespace render;

int Context::s_batchBitsNum = 0;
// this default value are for offset renderer
int Context::s_maxTextureUnits = 8;

namespace {
    Program program;
    GLFWwindow* window;

    StackAllocator _frameAllocator = StackAllocator(4000000);

    GLuint _vao = 0;

    float _points[] = {
            0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
    };

    GLuint _indecies[] = {
            0, 1, 2,
            3, 0, 2
    };

    void _init() {
#ifndef OFFSCREEN
//        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &Context::s_maxTextureUnits);
#endif
        Context::s_batchBitsNum = 0;
        auto i = Context::s_maxTextureUnits;
        while (i >>= 1)
            ++Context::s_batchBitsNum;
//        printf("GL_MAX_TEXTURE_IMAGE_UNITS = %d\n", Context::s_maxTextureUnits);
    }

    void _clear() {
        _frameAllocator.clear();
    }

    void _initVAO() {
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        GLuint indicesBuffer = 0;
        glGenBuffers(1, &indicesBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indecies), _indecies, GL_STATIC_DRAW);
    }

    void _draw(BufferData& bufData) {
        auto batchOffset = 0;
        for (auto drawIndex = 0; drawIndex < bufData.numDraws; ++drawIndex) {
            auto batchSize = bufData.batchSizes[drawIndex];
            glBindVertexArray(_vao);
            {
                int units = Context::s_maxTextureUnits;
                auto offset = drawIndex * units;
                for (unsigned i = 0; i < Context::s_maxTextureUnits; ++i) {
                    if (i + offset) {
                        glActiveTexture(GL_TEXTURE0 + i);
                        glBindTexture(GL_TEXTURE_2D, i + offset);
                    }
                }
            }

            glBindVertexArray(_vao);

            // TODO: move to const
            auto pointsSize = sizeof(_points);
            auto texturesSize = batchSize * sizeof(Context::TextureIndexType);
            auto matricesSize = batchSize * sizeof(Mat4);

            GLuint vertexBuffer = 0;
            glGenBuffers(1, &vertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, pointsSize + texturesSize + matricesSize, NULL, GL_STATIC_DRAW);

            glBufferSubData(GL_ARRAY_BUFFER, 0,                         pointsSize,   _points);
            glBufferSubData(GL_ARRAY_BUFFER, pointsSize,                texturesSize, bufData.textures + batchOffset);
            glBufferSubData(GL_ARRAY_BUFFER, pointsSize + texturesSize, matricesSize, bufData.matrices + batchOffset);

            auto matRowSize = 4 * sizeof(float);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glVertexAttribIPointer(1, 1, GL_INT, 0, (void*) pointsSize);
            auto matPoint = pointsSize + texturesSize;
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * matRowSize, (void*) (matPoint + 0 * matRowSize));
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * matRowSize, (void*) (matPoint + 1 * matRowSize));
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * matRowSize, (void*) (matPoint + 2 * matRowSize));
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matRowSize, (void*) (matPoint + 3 * matRowSize));

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
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, (GLsizei) batchSize);
            batchOffset += batchSize;
        }
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
    glClearColor(0.2, 0.2, 0.2, 1);
    glClearDepth(1.0f);
#else
//    const GLubyte* renderer = glGetString(GL_RENDERER);
//    const GLubyte* version = glGetString(GL_VERSION);
//    printf("Renderer: %s\n", renderer);
//    printf("OpenGL version supported %s\n", version);
    glReadBuffer(GL_BACK);
#endif

    _init();
    _initVAO();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    program.init();
    program.activate(nullptr);

    std::vector<int> samplers((unsigned long) s_maxTextureUnits);
    for (int i = 0; i < s_maxTextureUnits; ++i)
        samplers[i] = i;
    program.setUniform("u_textures", samplers.data(), s_maxTextureUnits);
}

void Context::start(DisplayObject& stage) {
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
        stage.preRender(renderState);

        BufferData bufData;
        RenderBufferOrganizer::organize(stage, _frameAllocator, bufData);

        _draw(bufData);

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
