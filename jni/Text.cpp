#include "gl_code.h"
#include <unistd.h>

void Text::Draw (GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    //LOGI("text %p %p %s",text,font,text);
    font->Write(text,x,y,xscale);
}

#define FONT_OFFSET 296
#define FONT_CHUNK_NUM 256
#define FONT_CHUNK_SIZE 14

Font::Font(const char *tex_filename,const char *fnt_filename) {
    FILE     *fnt_f;
    zip_file *file        = NULL;
    zip      *z           = NULL;
    int       i;
    int       err;
    error     status      = OK;
    GLfloat tex_coords[8] = {0};

    texture = new Image(tex_filename,1,1,standard_tex_coords);

    z = zip_open(DATA_DIR ZIP_FILENAME, 0, &err);
    if(NULL == z) {
        LOGI("Error opening %s\n",DATA_DIR ZIP_FILENAME);
        status = FILE_NOT_FOUND;
        goto delete_texture;
    }

    file = zip_fopen(z, fnt_filename, 0);
    if(NULL == file) {
        LOGI("Error opening %s from within zip\n",fnt_filename);
        status = FILE_NOT_FOUND;
        goto close_zip;
    }

    //There doesn't appear to be a zip_fseek function...
    for(i=0;i<FONT_OFFSET;i++) {
        if(1 != zip_fread(file,&err,1)) {
            LOGI("Error reading byte %d from font header\n",i);
            status = FILE_ERROR;
            goto close_zip_file;
        }
    }
        
    for(i = 0; i < FONT_CHUNK_NUM; i++) {
        GLfloat floats[FONT_CHUNK_SIZE];
        if((sizeof(float)*FONT_CHUNK_SIZE) != zip_fread(file,&floats,sizeof(float)*FONT_CHUNK_SIZE)) {
            LOGI("Error reading chunk %d from font header\n",i);
            status = FILE_ERROR;
            goto close_zip_file;
        }
            
        if(floats[1] == floats[3] && floats[2] == floats[4])
            continue;

        tex_coords[0] = floats[5];
        tex_coords[1] = floats[6];
        tex_coords[2] = floats[1];
        tex_coords[3] = floats[2];
        tex_coords[4] = floats[3];
        tex_coords[5] = floats[4];
        tex_coords[6] = floats[7];
        tex_coords[7] = floats[8];

        GLfloat width  = ((tex_coords[4]-tex_coords[2])*240/800);
        GLfloat height = (tex_coords[1]-tex_coords[3])/2;

        LOGI("letter %c : %.2f %.2f",(char)i,width*100,height*100);
        LOGI("%d [%.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f]",i,floats[0],floats[1],floats[2],floats[3],floats[4],floats[5],
             floats[6],floats[7],floats[8],floats[9],floats[10],floats[11],floats[12],floats[13]);

        //LOGI("Image tex coords %c : (%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)",i,tex_coords[0],tex_coords[1],tex_coords[2],tex_coords[3],tex_coords[4],tex_coords[5],tex_coords[6],tex_coords[7]);
        //LOGI("Image dimensions %c : (%.2f,%.2f)",i,width,height);

        letters[(char)i] = new ImagePtr(texture,width,height,tex_coords);
        offsets[(char)i] = (floats[10]-floats[13])/700.0; //why 700? no clue.
    }
close_zip_file:
    zip_fclose(file);
close_zip:
    zip_close(z);
delete_texture:
    if(status != OK && NULL != texture) {
        delete texture;
        texture = NULL;
    }
exit:
    if(status != OK) {
        string message("Error Loading font from file ");
        message += fnt_filename;
        LOGI("Error status %d processing font file %s\n",status,fnt_filename);
        throw ErrorMessage(status,message);
    } 
}

void Font::Write(const char *string,GLfloat x, GLfloat y, GLfloat size) {
    size_t len = strlen(string);
    size_t i;
    GLfloat sofar = 0;
    GLfloat kerning = 0.01;

    for(i=0;i<len;i++)
    { 
        letters_t::iterator current_letter = letters.find(string[i]);
        if(current_letter == letters.end())
            current_letter = letters.find('h');
        if(current_letter == letters.end())
            continue;

        offsets_t::iterator current_offset = offsets.find(string[i]);
        if(current_offset == offsets.end())
            current_offset = offsets.find('o');
        if(current_offset == offsets.end())
            continue;
        if(string[i] != ' ')  {
            //LOGI("char %c offset %.4f",string[i],current_offset->second);
            current_letter->second->Draw(sofar + x,y-current_offset->second,size,size);
        }
        sofar += (current_letter->second->width*size);
        //sofar += kerning;
    }
}
