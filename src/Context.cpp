#include "Contex.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "Program.h"
#include "DisplayObject.h"
#include <iostream>
#include "RenderState.h"
#include "Texture.h"

using Program = flash::render::Program;
using Texture = flash::display::Texture;

namespace {
    GLuint _vao;
    Program program;
    GLFWwindow* window;
}


#ifdef OFFSCREEN
#include <assert.h>
#include <png.h>
#include <jpeglib.h>

//extern void(*offscreenCallback)();
extern void offscreenCallback();

namespace {

    GLuint fbo;
    GLuint rbo_color;
    GLuint rbo_depth;

    unsigned w;
    unsigned h;

    enum Constants { SCREENSHOT_MAX_FILENAME = 256 };
    unsigned int nframes = 0;

    void prepareOffscreenBuffer() {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glGenRenderbuffers(1, &rbo_color);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, w, h);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color);

        glGenRenderbuffers(1, &rbo_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

        glReadBuffer(GL_COLOR_ATTACHMENT0);

        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER));
        int glget;
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &glget);
        assert(w * h < (unsigned int) glget);
    }

    void savePNG(const char *filename) {
        size_t i, nvals;
        const size_t format_nchannels = 3;
        FILE *f = fopen(filename, "wb");
        nvals = format_nchannels * w * h;

        GLubyte* pixels = (GLubyte*) malloc(nvals * sizeof(GLubyte));
        png_byte* png_bytes = (png_byte*) malloc(nvals * sizeof(png_byte));
        png_byte** png_rows = (png_byte**) malloc(h * sizeof(png_byte*));
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

    int saveJPEG(char* filename, unsigned width, unsigned height)
    {
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;

        /* this is a pointer to one row of image data */
        JSAMPROW row_pointer[1];
        FILE *outfile = fopen( filename, "wb" );

        if ( !outfile )
        {
            printf("Error opening output jpeg file %s\n!", filename );
            return -1;
        }
        cinfo.err = jpeg_std_error( &jerr );
        jpeg_create_compress(&cinfo);
        jpeg_stdio_dest(&cinfo, outfile);

        constexpr unsigned CH = 3;
        size_t bufSize = CH * width * height;
        unsigned char* bits = (unsigned char*) malloc(bufSize);
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, bits);

        /* Setting the parameters of the output file here */
        cinfo.image_width = width;
        cinfo.image_height = height;
        cinfo.input_components = CH;
        cinfo.in_color_space = JCS_RGB;
        /* default compression parameters, we shouldn't be worried about these */
        jpeg_set_defaults( &cinfo );
        /* Now do the compression .. */
        jpeg_start_compress( &cinfo, TRUE );
        size_t lastIndex = bufSize - cinfo.image_width * cinfo.input_components;
        /* like reading a file, this time write one row at a time */
        while( cinfo.next_scanline < cinfo.image_height )
        {
            row_pointer[0] = &bits[lastIndex - cinfo.next_scanline * cinfo.image_width * cinfo.input_components];
            jpeg_write_scanlines( &cinfo, row_pointer, 1 );
        }
        /* similar to read file, clean up after we're done compressing */
        jpeg_finish_compress( &cinfo );
        jpeg_destroy_compress( &cinfo );
        fclose( outfile );
        free(bits);
        /* success code is 1! */
        return 1;
    }

    void saveOffscreen() {
        glFlush();
        char filename[SCREENSHOT_MAX_FILENAME];

        snprintf(filename, SCREENSHOT_MAX_FILENAME, "tmp%d.png", nframes++);
        savePNG(filename);
        offscreenCallback();
//            saveJPEG("output.jpeg");
    }
}
#endif


using namespace flash;
using namespace render;

void Context::init(unsigned width, unsigned height) {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
//        return 1;
    }

#ifdef OFFSCREEN
    w = width;
    h = height;
#endif

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
#ifdef OFFSCREEN
        glfwHideWindow(window);
#endif
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

#ifdef OFFSCREEN
    prepareOffscreenBuffer();
#else
    glReadBuffer(GL_BACK);
#endif

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

        glfwPollEvents();

#ifdef OFFSCREEN
        saveOffscreen();
#else
        glfwSwapBuffers(window);
#endif
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