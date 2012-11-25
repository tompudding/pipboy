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
        free(data);
}

Image::Image(const char *filename, GLfloat _width, GLfloat _height, GLfloat *tex_coords) {
    data = NULL;
    width  = _width;
    height = _height;
    file_width = 512;
    file_height = 512;
    fname = (char *)filename;

    //Get image dimensions from the filename
    //should be like blahblah.512x512.raw
    char *a = strdup(filename);
    if(a) {
        char *p = strrchr(a,'.');
        if(p){
            *p = 0;
            char *q = strrchr(a,'.');
            if(q) {
                //check it has the dimensions string in it
                if(strstr(q,"dimensions_")) {
                    sscanf(q,".dimensions_%ux%u",&file_width,&file_height);
                    LOGI("Read dimensions of %u %u from %s",file_width,file_height,q);
                }
            }
        }
        free(a);
    }
    data = (uint8_t*)malloc(file_width*file_height*4);
    if(NULL == data)
        throw MEMORY_ERROR;
    memset(data,0xff,file_width*file_height*4);

    FILE *f = fopen(filename,"rb");
    if(NULL != f) {
        if(file_width*file_height*4 != fread(data,1,file_width*file_height*4,f)) {
            LOGE("Badness1\n");
            return;
        }
        fclose(f);
        RefreshTexture();
    }

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
    image_list.push_back(this); //can I do this?
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
