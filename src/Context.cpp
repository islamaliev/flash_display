#include "Contex.h"
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <cassert>
#include "RenderBufferOrganizer.h"
#include "Program.h"
#include "DisplayObject.h"
#include "StackAllocator.h"
#include "RenderState.h"
#include "Application.h"

using Mat4 = flash::math::Mat4;
using StackAllocator = flash::allocator::StackAllocator;
using Marker = StackAllocator::Marker;
using DisplayObject = flash::display::DisplayObject;

using namespace flash;
using namespace render;
using namespace ui;

int Context::s_batchBitsNum = 0;
// this default value is for offset renderer
int Context::s_maxTextureUnits = 8;

namespace {
    Program program;
    display::DisplayObject* _stage;

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
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &Context::s_maxTextureUnits);
#endif
        Context::s_batchBitsNum = 0;
        auto i = Context::s_maxTextureUnits;
        while (i >>= 1)
            ++Context::s_batchBitsNum;
//        printf("GL_MAX_TEXTURE_IMAGE_UNITS = %d\n", Context::s_maxTextureUnits);
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
#include <png.h>

extern const char* nextOffscreen();

namespace {

    GLuint _fbo = 0;
    GLuint _renderBuffer = 0;

    unsigned _w;
    unsigned _h;

    enum Constants { SCREENSHOT_MAX_FILENAME = 256 };

    void prepareOffscreenBuffer() {
        glGenFramebuffers(1, &_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

        glGenRenderbuffers(1, &_renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, _w, _h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderBuffer);

        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

        int glget;
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &glget);
        assert(_w * _h < (unsigned int) glget);
    }

    void savePNG(const char *filename) {
        size_t i, nvals;
        const size_t format_nchannels = 3;
        nvals = format_nchannels * _w * _h;
        FILE *f = fopen(filename, "wb");

        if (!f) {
            printf("Error opening output png file %s\n", filename );
            return;
        }

        GLubyte* pixels = (GLubyte*) malloc(nvals * sizeof(GLubyte));
        png_byte* png_bytes = (png_byte*) malloc(nvals * sizeof(png_byte));
        png_byte** png_rows = (png_byte**) malloc(_h * sizeof(png_byte*));
        glReadPixels(0, 0, _w, _h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        for (i = 0; i < nvals; i++)
            (png_bytes)[i] = (pixels)[i];
        for (i = 0; i < _h; i++)
            (png_rows)[_h - i - 1] = &(png_bytes)[i * _w * format_nchannels];
        png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png) abort();
        png_infop info = png_create_info_struct(png);
        if (!info) abort();
        if (setjmp(png_jmpbuf(png))) abort();
        png_init_io(png, f);
        png_set_IHDR(
                png,
                info,
                _w,
                _h,
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

bool _sync = false;

void _onAppVSync() {
    _sync = true;
}

void _onAppInit() {
    printf("_onAppInit\n");
    
#ifdef OFFSCREEN
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
    
    std::vector<int> samplers((unsigned long) Context::s_maxTextureUnits);
    for (int i = 0; i < Context::s_maxTextureUnits; ++i)
        samplers[i] = i;
    program.setUniform("u_textures", samplers.data(), Context::s_maxTextureUnits);
}

void Context::init(unsigned width, unsigned height) {
#ifdef OFFSCREEN
    _w = width;
    _h = height;
#endif
    Application::instance().run(&_onAppInit, &_onAppVSync);
}

void Context::start(DisplayObject& stage) {
    _stage = &stage;
    Application& app = Application::instance();
    
    while (app.isRunning()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef OFFSCREEN
        glViewport(0, 0, _w, _h);
        const char* name = nextOffscreen();
        if (strlen(name) == 0) {
            continue;
        }
#else
        if (!_sync)
            continue;
#endif

        _frameAllocator.clear();

        RenderState renderState;
        _stage->preRender(renderState);

        BufferData bufData;
        RenderBufferOrganizer::organize(*_stage, _frameAllocator, bufData);

        _draw(bufData);

#ifdef OFFSCREEN
        saveOffscreen(name);
#else
        app.swap();
        _sync = false;
#endif
    }
    
    dispose();
}

void Context::stop() {
    Application::instance().stop();
}

void Context::dispose() {
    glDeleteVertexArrays(1, &_vao);
    program.dispose();
#ifdef OFFSCREEN
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteRenderbuffers(1, &_fbo);
#endif
}

void Context::setProjection(const flash::math::Mat4& matrix) {
    program.setUniform("u_projection", matrix);
}
