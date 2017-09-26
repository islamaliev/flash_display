#include "FileLoader.h"
#include <istream>
#include <fstream>

#include <GL/glew.h>
#include <jpeglib.h>
#include <Color.h>
#include <Texture.h>


using namespace flash::filesystem;
using namespace std;

using Texture = flash::display::Texture;

namespace {
    Texture* decodeData(const char* FileName) {
        unsigned long x, y;

        unsigned long data_size;
        int channels;
        unsigned int type;
        unsigned char * rowptr[1];

        struct jpeg_decompress_struct info;
        struct jpeg_error_mgr err;

		FILE* file;
		errno_t file_err = fopen_s(&file, FileName, "rb");

        info.err = jpeg_std_error(& err);
        jpeg_create_decompress(& info);

        if(file_err != 0) {
            fprintf(stderr, "Error reading JPEG file %s!", FileName);
            return 0;
        }

        jpeg_stdio_src(&info, file);
        jpeg_read_header(&info, TRUE);

        jpeg_start_decompress(&info);

        x = info.output_width;
        y = info.output_height;
        channels = info.num_components;
        type = GL_RGB;
        if(channels == 4) type = GL_RGBA;

        data_size = x * y * 3;

        Texture* texture = new flash::display::Texture(x, y);

        unsigned char * jdata = (unsigned char*) texture->data();
        while (info.output_scanline < info.output_height) // loop
        {
            rowptr[0] = (unsigned char*) jdata
                       + 3 * info.output_width * info.output_scanline;

            jpeg_read_scanlines(&info, rowptr, 1);
        }

        jpeg_finish_decompress(&info);

        fclose(file);

        texture->bindData();

        return texture;
    }
}

void FileLoader::load(const char* path) {
    clear();
    streampos size;

    if(strstr(path, ".jpg") - path == strlen(path) - 4) {
        Texture* texture = decodeData(path);
        m_data = texture;
        m_size = texture->width() * texture->height() * sizeof(Color);
        return;
    }
    
    ifstream file (path, ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        m_size = (size_t) size;
        m_data = new char [size];
        file.seekg(0, ios::beg);
        file.read((char*) m_data, size);
        file.close();
    } else {

    }
}

FileLoader::~FileLoader() {
    clear();
}

void FileLoader::clear() {
    if (m_size) {
        m_size = 0;
        // TODO: figure out how to handle data that wasn't requested after loading
//        delete (char*) m_data;
    }
}
