#include <png.h>
#include <GL/glew.h>
#include <cstdlib>
#include <cstring>
#include <cassert>

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
		FILE* fileStream;
		errno_t err = fopen_s(&fileStream, filename, "wb");

		if (err != 0) {
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
		png_init_io(png, fileStream);
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
		fclose(fileStream);
	}
    
    void saveOffscreen(const char* name) {
        glFlush();
        char filename[SCREENSHOT_MAX_FILENAME];
        strcpy(filename, name);
        strcat(filename, ".png");
        savePNG(filename);
    }
    
    class RenderMediator {
    public:
        void appInit() {
            prepareOffscreenBuffer();
			glClearColor(0.2f, 0.2f, 0.2f, 1);
			glClearDepth(1.0f);
        }
        
        void initWindow(unsigned width, unsigned height) {
            _w = width;
            _h = height;
            Application::instance().run(&_onAppInit, nullptr);
        }
        
        bool preRender() {
            glViewport(0, 0, _w, _h);
            m_name = nextOffscreen();
            return strlen(m_name) != 0;
        }
        
        void postRender() {
            saveOffscreen(m_name);
        }
        
        void dispose() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteRenderbuffers(1, &_fbo);
        }
    
    private:
        const char* m_name;
    };
}