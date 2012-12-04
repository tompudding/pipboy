#include "gl_code.h"
#include <unistd.h>
#include <list>

std::list<Image*> image_list;

ImagePtr::ImagePtr(Image *orig, GLfloat _width, GLfloat _height, GLfloat *tex_coords) {
    width  = _width;
    height = _height;
    GLfloat coords[] = {
        // X, Y, Z
        0, 0, 0,
        0, height, 0,
        width, height, 0,
        width, 0, 0
    };
    GLshort indicies[] = {0,1,2,3};
    memcpy(mFVertexBuffer,coords,sizeof(mFVertexBuffer));
    memcpy(mTexBuffer,tex_coords,sizeof(mTexBuffer));
    memcpy(mIndexBuffer,indicies,sizeof(mIndexBuffer));
    imageptr = orig;
}

void ImagePtr::Draw(GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    glLoadIdentity();checkGlError(__LINE__);
    glTranslatef(x,y,0);checkGlError(__LINE__);
    if(xscale != 1.0 or yscale != 1.0) {
        glScalef(xscale,yscale,1.0);
    }

    glBindTexture(GL_TEXTURE_2D, imageptr->texture);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                       GL_REPEAT);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                       GL_REPEAT);

    glFrontFace(GL_CCW);checkGlError(__LINE__);
    glVertexPointer(3,GL_FLOAT, 0, mFVertexBuffer);checkGlError(__LINE__);
    glEnable(GL_TEXTURE_2D);checkGlError(__LINE__);
    glTexCoordPointer(2, GL_FLOAT, 0, mTexBuffer);checkGlError(__LINE__);
    glDrawElements(GL_TRIANGLE_FAN, 4,
                   GL_UNSIGNED_SHORT, mIndexBuffer);checkGlError(__LINE__);
}

Image::~Image() {
    if(data != NULL)
        delete[] data;
}

void png_zip_read(png_structp png, png_bytep data, png_size_t size)
{
    zip_file* zfp = (zip_file*)png_get_io_ptr(png);
    zip_fread(zfp, data, size);
}

Image::Image(const char *filename, GLfloat _width, GLfloat _height, GLfloat *tex_coords) {
    zip_file    *file         = NULL;
    error        status       = OK;
    png_infop    info_ptr     = NULL;
    png_structp  png_ptr      = NULL;
    png_infop    end_info     = NULL;
    png_bytep   *row_pointers = NULL;
    png_byte     header[8];
    zip         *z            = NULL;
    int          err          = 0;
    size_t rowbytes;
    //variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 twidth, theight;
    
    GLfloat coords[] = {
        // X, Y, Z
        0, 0, 0,
        0, _height, 0,
        _width, _height, 0,
        _width, 0, 0
    };
    
    GLshort indicies[] = {0,1,2,3};

    data   = NULL;
    width  = _width;
    height = _height;
    fname  = (char *)filename;

    z = zip_open(DATA_DIR ZIP_FILENAME, 0, &err);
    if(NULL == z) {
        LOGI("Error opening %s\n",DATA_DIR ZIP_FILENAME);
        status = FILE_NOT_FOUND;
        goto exit;
    }
        
    file = zip_fopen(z, filename, 0);
    if(NULL == file) {
        LOGI("Error opening %s from within zip\n",filename);
        status = FILE_NOT_FOUND;
        goto exit;
    }

    
    if(8 != zip_fread(file, header, 8)) {
        LOGI("Error reading header for %s\n",filename);
        status = FILE_ERROR;
        goto close_zip;
    }

    if(png_sig_cmp(header, 0, 8)) {
        LOGI("invalid header for %s\n",filename);
        status = FILE_ERROR;
        goto close_zip;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (NULL == png_ptr) {
        LOGI("png_ptr error for %s\n",filename);
        status = MEMORY_ERROR;
        goto close_zip;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (NULL == info_ptr) {
        LOGI("info_ptr error for %s\n",filename);
        end_info  = NULL;
        status = MEMORY_ERROR;
        goto free_png_ptr;
    }

    end_info = png_create_info_struct(png_ptr);
    if(NULL == end_info) {
        LOGI("end_info error for %s\n",filename);
        status = MEMORY_ERROR;
        goto free_png_ptr;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        LOGI("processing_error for %s\n",filename);
        status = FILE_ERROR;
        goto free_data;
    }

    //init png reading
    //png_init_io(png_ptr, fp);
    png_set_read_fn(png_ptr, file, png_zip_read);

    //let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
                 NULL, NULL, NULL);

    file_width  = twidth;
    file_height = theight;

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // Allocate the image_data as a big block, to be given to opengl
    if(rowbytes > 0x00010000) {
        status = MEMORY_ERROR;
        goto free_png_ptr;
    }
    data = new png_byte[rowbytes * theight];
    if (!data) {
        //clean up memory and close stuff
        status = MEMORY_ERROR;
        goto free_data;
    }
    //row_pointers is for pointing to image_data for reading the png with libpng
    row_pointers = new png_bytep[theight];
    if (NULL == row_pointers) {
        status = MEMORY_ERROR;
        goto free_data;
    }
    // set the individual row_pointers to point at the correct offsets of image_data
    for (int i = 0; i < theight; ++i)
        row_pointers[theight - 1 - i] = data + i * rowbytes;
    //read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);
    RefreshTexture();

    memcpy(mFVertexBuffer,coords,sizeof(mFVertexBuffer));
    memcpy(mTexBuffer,tex_coords,sizeof(mTexBuffer));
    memcpy(mIndexBuffer,indicies,sizeof(mIndexBuffer));
    image_list.push_back(this); //can I do this?
    
free_data:
    if(status != OK and NULL != data) {
        delete[] data;
        data = NULL;
    }
    if(NULL != row_pointers) {
        delete[] row_pointers;
    }
free_png_ptr:
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
close_zip:
    zip_fclose(file);
exit:
    if(status != OK) {
        LOGI("Error status %d processing file %s\n",status,filename);
        throw status;
    }
}

void Image::RefreshTexture() {
    texture = GenTexture(file_width,file_height,data);
    LOGE("Refreshing texture:%s",fname);
}

void Image::Draw(GLfloat x, GLfloat y,GLfloat xscale, GLfloat yscale) {
    glLoadIdentity();checkGlError(__LINE__);
    glTranslatef(x,y,0);checkGlError(__LINE__);
    if(xscale != 1.0 or yscale != 1.0) {
        glScalef(xscale,yscale,1.0);
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                       GL_REPEAT);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                       GL_REPEAT);

    glFrontFace(GL_CCW);checkGlError(__LINE__);
    glVertexPointer(3,GL_FLOAT, 0, mFVertexBuffer);checkGlError(__LINE__);
    glEnable(GL_TEXTURE_2D);checkGlError(__LINE__);
    glTexCoordPointer(2, GL_FLOAT, 0, mTexBuffer);checkGlError(__LINE__);
    glDrawElements(GL_TRIANGLE_FAN, 4,
                   GL_UNSIGNED_SHORT, mIndexBuffer);checkGlError(__LINE__);
}

void RefreshImages() {
    for(std::list<Image*>::iterator i = image_list.begin(); i != image_list.end(); i++) {
        if(*i != NULL)
            (*i)->RefreshTexture();
    }
}
