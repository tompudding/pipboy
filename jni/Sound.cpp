#include "gl_code.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void LoadSoundFile(const char *filename,short **buffer,size_t *size) {
    //FILE *data = NULL;
    struct zip_stat st = {0};
    zip_file    *file         = NULL;
    zip         *z            = NULL;
    char temp_path[1024] = "sounds/";
    error status = OK;
    int err;
    
    strncat(temp_path,filename,sizeof(temp_path) - strlen(temp_path));
    
    z = zip_open(DATA_DIR ZIP_FILENAME, 0, &err);
    if(NULL == z) {
        LOGI("Error opening %s\n",DATA_DIR ZIP_FILENAME);
        status = FILE_NOT_FOUND;
        goto exit;
    }

    if(zip_stat(z,temp_path,0,&st)) {
        status = FILE_NOT_FOUND;
        goto close_zip;
    }

    *buffer = (short int *)malloc(st.size);
    if(NULL == *buffer) {
        status = MEMORY_ERROR;
        goto close_zip;
    }

    file = zip_fopen(z, temp_path, 0);
    if(NULL == file) {
        LOGI("Error opening %s from within zip\n",temp_path);
        status = FILE_NOT_FOUND;
        goto free_data;
    }

    if(st.size != zip_fread(file, *buffer, st.size)) {
        LOGI("Error reading header for %s\n",filename);
        status = FILE_ERROR;
        goto free_data;
    }

    *size = st.size;

free_data:
    if(status != OK and NULL != *buffer) {
        free(*buffer);
        *buffer = NULL;
    }
close_zip_file:
    zip_fclose(file);
close_zip:
    zip_close(z);
exit:
    if(status != OK) {
        LOGI("Error status %d processing file %s\n",status,temp_path);
        throw status;
    }
}

SoundClip::SoundClip(const char *filename) {
    LoadSoundFile(filename,&buffer,&size);
}

SoundClip::~SoundClip() {
    if(NULL != buffer) 
        free(buffer);
}

void SoundClip::GetBuffer(short **out,size_t *outs) {
    *out = buffer;
    *outs = size;
}

RandomSoundClip::RandomSoundClip(char **filename) {
    buffer = NULL;
    size   = NULL;
    count = 0;
    try {
        if(NULL == filename)
            throw MEMORY_ERROR;
        //count how many we want
        for(char **p = filename; *p; p++)
            count ++;

        LOGI("Got %u rs filenames",count);

        if(count > 256)
            throw MEMORY_ERROR;

        buffer = (short **)malloc(sizeof(short*)*count);
        if(NULL == buffer)
            throw MEMORY_ERROR;
        memset(buffer,0,sizeof(short*)*count);

        size   = (size_t*)malloc(sizeof(size_t)*count);
        if(NULL == size)
            throw MEMORY_ERROR;
        memset(size,0,sizeof(size_t)*count);
        int i = 0;
        for(char **p = filename; *p; p++,i++) 
            LoadSoundFile(*p,&(buffer[i]),&(size[i]));
    }
    catch (error e) {
        if(NULL != buffer) {
            for(int i=0;i<count;i++) {
                if(NULL != buffer[i]) {
                    free(buffer[i]);
                    buffer[i] = NULL;
                }
            }
            free(buffer);
            buffer = NULL;
        }
        if(NULL != size) {
            free(size);
            size = NULL;
        }
        throw e;
    }
}

RandomSoundClip::~RandomSoundClip() {
    if(NULL != buffer) {
        for(int i=0;i<count;i++) {
            if(NULL != buffer[i]) {
                free(buffer[i]);
                buffer[i] = NULL;
            }
        }
        free(buffer);
        buffer = NULL;
    }
    if(NULL != size) {
        free(size);
        size = NULL;
    }
}

void RandomSoundClip::GetBuffer(short **outbuffer, size_t *outsize) {
    unsigned long val = lrand48();
    val = val%count;
    //LOGI("Choosing item %d",val);
    
    *outbuffer = buffer[val];
    *outsize   = size[val];
}

void PlayClip(SoundClip &sc,int queue,int q) {
    short *buffer;
    size_t size;
    sc.GetBuffer(&buffer,&size);
    if(NULL != buffer)
        PlayClip_c(buffer,size,queue,q);
}
