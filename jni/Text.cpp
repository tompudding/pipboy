#include "gl_code.h"
#include <unistd.h>

void Text::Draw (GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    //LOGI("text %p %p %s",text,font,text);
    font->Write(text,x,y,xscale);
}

Font::Font(const char *tex_filename,const char *fnt_filename) {
    FILE *fnt_f;
    int i;

    texture = new Image(tex_filename,1,1,standard_tex_coords);

    fnt_f = fopen(fnt_filename,"rb");
    try {
        GLfloat tex_coords[8] = {0};
        if(NULL == fnt_f) {
            LOGI("%d",__LINE__);
            throw FILE_NOT_FOUND;
        }
        
        if(0 != fseek(fnt_f,296,SEEK_SET)) 
            throw FILE_ERROR;
        
        for(i = 0; i < 256; i++) {
            GLfloat floats[14];
            if(14 != fread(&floats,4,14,fnt_f))
                throw FILE_ERROR;
            
            if(floats[1] == floats[3] && floats[2] == floats[4])
                continue;

            tex_coords[0] = floats[5]/2.0;
            tex_coords[1] = floats[6]/2.0;
            tex_coords[2] = floats[1]/2.0;
            tex_coords[3] = floats[2]/2.0;
            tex_coords[4] = floats[3]/2.0;
            tex_coords[5] = floats[4]/2.0;
            tex_coords[6] = floats[7]/2.0;
            tex_coords[7] = floats[8]/2.0;

            GLfloat width  = (tex_coords[4]-tex_coords[2])*480/800;
            GLfloat height = (tex_coords[1]-tex_coords[3]);

            //LOGI("letter %c : %.2f %.2f",(char)i,width*100,height*100);
            //LOGI("%d [%.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f]",i,floats[0],floats[1],floats[2],floats[3],floats[4],floats[5],
            //     floats[6],floats[7],floats[8],floats[9],floats[10],floats[11],floats[12],floats[13]);

            //LOGI("Image tex coords %c : (%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)",i,tex_coords[0],tex_coords[1],tex_coords[2],tex_coords[3],tex_coords[4],tex_coords[5],tex_coords[6],tex_coords[7]);
            //LOGI("Image dimensions %c : (%.2f,%.2f)",i,width,height);

            letters[(char)i] = new ImagePtr(texture,width,height,tex_coords);
            offsets[(char)i] = (floats[10]-floats[13])/700.0; //why 700? no clue.
        }
        fclose(fnt_f);
    }
    catch(error e) {
        LOGI("%d",__LINE__);
        if(NULL != fnt_f) {
            fclose(fnt_f);
        }
        throw e;
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