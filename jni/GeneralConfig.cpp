#include "gl_code.h"

GeneralConfig::GeneralConfig(const char *filename) {

    FILE *f;
    error result = OK;
    int i;
    string error_message;
    char buffer[1024] = {0};
    uint32_t *number_targets[8] = {&level,
                                   &hp.current,
                                   &hp.max,
                                   &ap.current,
                                   &ap.max,
                                   &xp.current,
                                   &xp.max,
                                   &caps};
    string *string_targets[2] = {&name,&location};
    /* // Some sensible default
    rgb[0]     = 0.54f;
    rgb[1]     = 0.43f;
    rgb[2]     = 0.19f;
    level      = 20;
    hp.current = 419;
    hp.max     = 460;
    ap.current = 35;
    ap.max     = 70;
    xp.current = 29200;
    xp.max     = 29450;
    name       = "Tom Glooping";
    location   = "Here and now";
    caps       = 61; */
    
    f = fopen(DATA_DIR "config.txt","rb");
    if(NULL == f) {
        result = FILE_NOT_FOUND;
        error_message = "Error opening ";
        error_message += DATA_DIR "config.txt";
        goto finish;
    }

    for(i=0;i<3;i++) {
        if(NULL == fgets(buffer,sizeof(buffer),f)) {
            //file ran out before we got all the data
            result = FILE_ERROR;
            error_message = "Error parsing ";
            error_message += DATA_DIR "config.txt";
            goto close_file;
        }
        rgb[i] = strtof(buffer,NULL);
        if(rgb[i] < 0) {
            rgb[i] = 0;
        }
        if(rgb[i] > 1) {
            rgb[i] = 1;
        }
    }
    for(i=0;i<8;i++) {
        if(NULL == fgets(buffer,sizeof(buffer),f)) {
            //file ran out before we got all the data
            result = FILE_ERROR;
            error_message = "Error parsing ";
            error_message += DATA_DIR "config.txt";
            goto close_file;
        }
        *(number_targets[i]) = strtoul(buffer,NULL,10);
    }
    //Now for the two strings
    for(i=0;i<2;i++) {
        char *hash_ptr = NULL;
        if(NULL == fgets(buffer,sizeof(buffer),f)) {
            //file ran out before we got all the data
            result = FILE_ERROR;
            error_message = "Error parsing ";
            error_message += DATA_DIR "config.txt";
            goto close_file;
        }
        //Truncate at the comment symbol
        if(hash_ptr = strchr(buffer,'#')) {
            *hash_ptr = '\0';
        }
        *(string_targets[i]) = buffer;
    }
close_file:
    if(NULL != f) {
        fclose(f);
    }

finish:
    if(result != OK) {
        throw ErrorMessage(result,error_message);
    }
}
