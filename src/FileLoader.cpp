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

        unsigned long data_size;     // length of the file
        int channels;               //  3 =>RGB   4 =>RGBA
        unsigned int type;
        unsigned char * rowptr[1];    // pointer to an array

        struct jpeg_decompress_struct info; //for our jpeg info
        struct jpeg_error_mgr err;          //the error handler

        FILE* file = fopen(FileName, "rb");  //open the file

        info.err = jpeg_std_error(& err);
        jpeg_create_decompress(& info);   //fills info structure

        //if the jpeg file doesn't load
        if(!file) {
            fprintf(stderr, "Error reading JPEG file %s!", FileName);
            return 0;
        }

        jpeg_stdio_src(&info, file);
        jpeg_read_header(&info, TRUE);   // read jpeg file header

        jpeg_start_decompress(&info);    // decompress the file

        //set width and height
        x = info.output_width;
        y = info.output_height;
        channels = info.num_components;
        type = GL_RGB;
        if(channels == 4) type = GL_RGBA;

        data_size = x * y * 3;

        Texture* texture = new Texture(x, y);

        //--------------------------------------------
        // read scanlines one at a time & put bytes
        //    in jdata[] array. Assumes an RGB image
        //--------------------------------------------
        unsigned char * jdata = (unsigned char*) texture->data();        // data for the image
        while (info.output_scanline < info.output_height) // loop
        {
            // Enable jpeg_read_scanlines() to fill our jdata array
            rowptr[0] = (unsigned char *)jdata +  // secret to method
                        3* info.output_width * info.output_scanline;

            jpeg_read_scanlines(&info, rowptr, 1);
        }
        //---------------------------------------------------

        jpeg_finish_decompress(&info);   //finish decompressing

        //----- create OpenGL tex map (omit if not needed) --------
//        unsigned int texture_id;
//        glGenTextures(1,&texture_id);
//        glBindTexture(GL_TEXTURE_2D, texture_id);
//        gluBuild2DMipmaps(GL_TEXTURE_2D,3,x,y,GL_RGB,GL_UNSIGNED_BYTE,jdata);

        fclose(file);                    //close the file

        return texture;    // for OpenGL tex maps
    }
}

void FileLoader::load(const char* path) {
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
    if (m_size) {
        delete (char*) m_data;
        m_size = 0;
    }
}
