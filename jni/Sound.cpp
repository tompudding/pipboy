#include "gl_code.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

std::list<SoundClip*> sound_list;

void LoadSoundFile(const char *prefix,const char *filename,short **buffer,size_t *size) {
    //FILE *data = NULL;
    struct zip_stat st = {0};
    zip_file    *file         = NULL;
    zip         *z            = NULL;
    char temp_path[1024]      = {0};
    error status = OK;
    int err;

    if(sizeof(temp_path) <= snprintf(temp_path,sizeof(temp_path),"%s/%s",prefix,filename)) {
        status = MEMORY_ERROR;
        goto exit;
    }
    temp_path[sizeof(temp_path)-1] = 0;
    
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
        string message("Error Loading sound from file ");
        message += temp_path;

        LOGI("Error status %d processing file %s\n",status,temp_path);
        throw ErrorMessage(status,message);
    }
}

size_t GetSizeSound(const char *filename) {
    int              err = 0;
    struct zip_stat  st  = {0};
    zip             *z   = NULL;
    size_t           out = 0;
    char temp_path[1024] = "sounds/";

    strncat(temp_path,filename,sizeof(temp_path) - strlen(temp_path));

    //Get the size of the file for helping make the loading bar more accurate.
    z = zip_open(DATA_DIR ZIP_FILENAME, 0, &err);
    if(NULL == z) {
        LOGI("Error opening %s for size computation\n",DATA_DIR ZIP_FILENAME);
        goto exit;
    }
    if(zip_stat(z,temp_path,0,&st)) {
        goto close_zip;
    }
    out = st.size;
    LOGI("Got size %u for sound %s",out,filename);
close_zip:
    zip_close(z);
exit:
    return out;
}

SoundClip::SoundClip(const char *filename) {
    fname = strdup(filename);
    prefix = "sounds";
    loaded = false;
    size = 1;
    //size = GetSizeSound(filename);

    sound_list.push_back(this); //can I do this?
}

MusicFile::MusicFile(const char *filename) {
    fname = strdup(filename);
    prefix = "music";
    loaded = false;
    size = 1;
    //size = GetSizeSound(filename);

    sound_list.push_back(this); //can I do this?
}


void SoundClip::Load() {
    LoadSoundFile(prefix,fname,&buffer,&size);
    loaded = true;
}

size_t SoundClip::Size() {
    return size;
}

SoundClip::~SoundClip() {
    if(NULL != buffer) 
        free(buffer);
    if(NULL != fname) {
        free(fname);
    }
}

void SoundClip::GetBuffer(short **out,size_t *outs) {
    *out = buffer;
    *outs = size;
}

RandomSoundClip::RandomSoundClip(char **filename) {
    buffer     = NULL;
    size       = NULL;
    count      = 0;
    loaded     = false;
    prefix     = "sounds";
    total_size = 0;
    int i      = 0;
    //make a copy of the filenames for later loading
    for(char **p = filename; *p; p++)
        count++;
    filenames = (char**)malloc(sizeof(char*)*count);
    if(NULL == filenames) {
        throw ErrorMessage(MEMORY_ERROR,"Out of Memory");
    }
    for(char **p = filename; *p; p++,i++) {
        filenames[i] = strdup(*p);
        //total_size  += GetSizeSound(*p);
        total_size += 1;
    }

    sound_list.push_back(this); //can I do this?
}

size_t RandomSoundClip::Size() {
    return total_size;
}


void RandomSoundClip::Load() {
    try {
        LOGI("Got %u rs filenames",count);

        if(count > 256)
            throw ErrorMessage(MEMORY_ERROR,"Out of Memory");

        buffer = (short **)malloc(sizeof(short*)*count);
        if(NULL == buffer)
            throw ErrorMessage(MEMORY_ERROR,"Out of Memory");
        memset(buffer,0,sizeof(short*)*count);

        size   = (size_t*)malloc(sizeof(size_t)*count);
        if(NULL == size)
            throw ErrorMessage(MEMORY_ERROR,"Out of Memory"); 
        memset(size,0,sizeof(size_t)*count);
        int i = 0;
        for(i=0;i<count;i++) {
            LoadSoundFile(prefix,filenames[i],&(buffer[i]),&(size[i]));
        }
        loaded = true;
    }
    //This is a big lame that we have to do this. Hacks all round
    catch (ErrorMessage e) {
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

    if(!loaded) {
        throw ErrorMessage(UNINITIALISED,"Uninitialised Randomsoundclip used");
    }

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

void LoadSounds(JNIEnv *env, jobject callbackClass, jmethodID progress_method, size_t *loaded, size_t total_items) {
    LOGI("Loadimages with %d sounds\n",sound_list.size());
    for(std::list<SoundClip*>::iterator i = sound_list.begin(); i != sound_list.end(); i++) {
        if(*i != NULL) {
            (*i)->Load();
            if(progress_method) {                    
                (*loaded) += 1;//(*i)->Size();                                            
                env->CallVoidMethod(callbackClass,progress_method,((float)(*loaded))/total_items); 
            }
        }
    }
}

size_t NumSounds() {
    size_t num = 0;
    for(std::list<SoundClip*>::iterator i = sound_list.begin(); i != sound_list.end(); i++) {
        if(*i != NULL) {
            num += 1;//(*i)->Size();
        }
    }
    return num;
}
