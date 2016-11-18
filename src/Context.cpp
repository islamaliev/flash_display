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
    
    void _onAppInit();
}

#ifndef OFFSCREEN
namespace {
    
    bool _sync = false;
    
    void _onAppVSync() {
        _sync = true;
    }
    
    class RenderMediator {
    public:
        void appInit() {
//          const GLubyte* renderer = glGetString(GL_RENDERER);
//          const GLubyte* version = glGetString(GL_VERSION);
//          printf("Renderer: %s\n", renderer);
//          printf("OpenGL version supported %s\n", version);
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &Context::s_maxTextureUnits);
            glReadBuffer(GL_BACK);
        }
        
        void initWindow(unsigned width, unsigned height) {
            Application::instance().run(&_onAppInit, &_onAppVSync);
        }
        
        bool preRender() {
            return _sync;
        }
        
        void postRender() {
            Application::instance().swap();
            _sync = false;
        }
        
        void dispose() {
        }
    };
}
#else
#include "OffscreenRenderering.h"
#endif

namespace {
    RenderMediator _renderMediator;
    
    void _onAppInit() {
        printf("_onAppInit\n");
        
        _renderMediator.appInit();
        
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
}

void Context::init(unsigned width, unsigned height) {
    _renderMediator.initWindow(width, height);
}

void Context::start(DisplayObject& stage) {
    _stage = &stage;
    Application& app = Application::instance();
    
    while (app.isRunning()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!_renderMediator.preRender())
            continue;
        
        _frameAllocator.clear();

        RenderState renderState;
        _stage->preRender(renderState);

        BufferData bufData;
        RenderBufferOrganizer::organize(*_stage, _frameAllocator, bufData);

        _draw(bufData);

        _renderMediator.postRender();
    }
    
    dispose();
}

void Context::stop() {
    Application::instance().stop();
}

void Context::dispose() {
    glDeleteVertexArrays(1, &_vao);
    program.dispose();
    _renderMediator.dispose();
}

void Context::setProjection(const flash::math::Mat4& matrix) {
    program.setUniform("u_projection", matrix);
}
