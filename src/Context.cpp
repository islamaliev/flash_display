#include "Contex.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "Program.h"
#include "DisplayObject.h"
#include <iostream>
#include <RenderState.h>
#include "Texture.h"
#include <assert.h>


#include <png.h>
#include "FreeImage.h"


using Program = flash::render::Program;
using Texture = flash::display::Texture;

namespace {
    GLuint _vao;
    Program program;
    GLFWwindow* window;

    bool offscreen = true;

    GLubyte *pixels = NULL;
    GLuint fbo;
    GLuint rbo_color;
    GLuint rbo_depth;

    unsigned w;
    unsigned h;

    png_byte *png_bytes = NULL;
    png_byte **png_rows = NULL;

    enum Constants { SCREENSHOT_MAX_FILENAME = 256 };
    unsigned int nframes = 0;


    void screenshot_png(const char *filename, unsigned int width, unsigned int height,
                               GLubyte **pixels, png_byte **png_bytes, png_byte ***png_rows) {
        size_t i, nvals;
        const size_t format_nchannels = 4;
        FILE *f = fopen(filename, "wb");
        nvals = format_nchannels * width * height;
        *pixels = (GLubyte*) realloc(*pixels, nvals * sizeof(GLubyte));
        *png_bytes = (png_byte*) realloc(*png_bytes, nvals * sizeof(png_byte));
        *png_rows = (png_byte**) realloc(*png_rows, height * sizeof(png_byte*));
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, *pixels);
        for (i = 0; i < nvals; i++)
            (*png_bytes)[i] = (*pixels)[i];
        for (i = 0; i < height; i++)
            (*png_rows)[height - i - 1] = &(*png_bytes)[i * width * format_nchannels];
        png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png) abort();
        png_infop info = png_create_info_struct(png);
        if (!info) abort();
        if (setjmp(png_jmpbuf(png))) abort();
        png_init_io(png, f);
        png_set_IHDR(
                png,
                info,
                width,
                height,
                8,
                PNG_COLOR_TYPE_RGBA,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT
        );
        png_write_info(png, info);
        png_write_image(png, *png_rows);
        png_write_end(png, NULL);
        fclose(f);
    }

    void save(const char* filename, unsigned int width, unsigned int height, GLubyte** pixels) {
        FreeImage_Initialise();

        constexpr unsigned CH = 3;
        size_t bufSize = CH * width * height;

        unsigned char* bits = (unsigned char*) realloc(*pixels, bufSize);
        glReadPixels(0,0, width, height, GL_BGR, GL_UNSIGNED_BYTE, bits);

        unsigned int pitch = ((width * CH * 8 + 31) / 32) * 4;
        FIBITMAP* img = FreeImage_ConvertFromRawBits(bits, width, height, pitch,
                           CH * 8, 0xFF0000, 0x00FF00, 0x0000FF, false);
        if (FreeImage_Save(FIF_BMP, img, "/Users/islamaliev/dev/testing/cpp/flash_display/build/output.bmp", 0)) {
            std::cout << "YEAHAAAAAAA!" << std::endl;
        } else {
            std::cout << "JOPA" << std::endl;
        }

        FreeImage_DeInitialise();
    }
}

using namespace flash;
using namespace render;

void Context::init(unsigned width, unsigned height) {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
//        return 1;
    }

    w = width;
    h = height;

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
    if (offscreen) {
        glfwHideWindow(window);
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);





    if (offscreen) {
        int glget;
        /*  Framebuffer */
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        /* Color renderbuffer. */
        glGenRenderbuffers(1, &rbo_color);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
        /* Storage must be one of: */
        /* GL_RGBA4, GL_RGB565, GL_RGB5_A1, GL_DEPTH_COMPONENT16, GL_STENCIL_INDEX8. */
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, width, height);
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB5_A1, width, height);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color);

        /* Depth renderbuffer. */
        glGenRenderbuffers(1, &rbo_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

        glReadBuffer(GL_COLOR_ATTACHMENT0);

        /* Sanity check. */
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER));
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &glget);
        assert(width * height < (unsigned int)glget);
    } else {
        glReadBuffer(GL_BACK);
    }






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
//        glfwSwapBuffers(window);



        if (offscreen) {
            glFlush();
            char filename[SCREENSHOT_MAX_FILENAME];

            snprintf(filename, SCREENSHOT_MAX_FILENAME, "tmp%d.png", nframes);
//          screenshot_png(filename, w, h, &pixels, &png_bytes, &png_rows);
            save(filename, w, h, &pixels);
            return;
        } else {
            glfwSwapBuffers(window);
        }

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

bool textInit = false;

void Context::setTexture(const Texture* texture) {
    if (!textInit)
    {
        unsigned int texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, texture->width(), texture->height());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture->width(), texture->height(), GL_RGB, GL_UNSIGNED_BYTE, texture->data());
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        program.setUniform("s", 0);
        textInit = true;
    }
}