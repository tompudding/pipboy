#include "gl_code.h"
#include <unistd.h>
#include <SLES/OpenSLES.h>
#include <stdlib.h>

Image *glare     = NULL;
Image *band      = NULL;
Image *scanlines = NULL;
Image *fade      = NULL;
Image *icon_explosives = NULL;
Image *icon_energy = NULL ;
Image *icon_bigguns = NULL;
Image *icon_unarmed = NULL;
Image *icon_sm_arms = NULL;
Image *icon_melee = NULL;
Image *arrow      = NULL;
Font *font       = NULL;
ViewList *viewlist = NULL;
GLuint white_texture = 0;
GLuint grey_texture = 0;
SoundClip *mode_change = NULL;
SoundClip *menu_tab = NULL;
SoundClip *mode_change_buzz = NULL;
SoundClip *menu_prevnext = NULL;
SoundClip *equip_weapon_sound = NULL;
SoundClip *equip_apparel_sound = NULL;
SoundClip *equip_aid_sound = NULL;
SoundClip *equip_misc_sound = NULL;
SoundClip *unequip_weapon_sound = NULL;
SoundClip *unequip_apparel_sound = NULL;
SoundClip *select_sound = NULL;
ItemList *item_list = NULL;
Text *general_text = NULL;
ItemConditionBar *general_condition_bar = NULL;
int initialised = 0;
struct timeval tv = {0};

GLfloat standard_tex_coords[] = {0, 1.0,
                                 0, 0.0,
                                 1.0, 0.0,
                                 1.0, 1.0};

void checkGlError(int op) {
    GLint error;
    for (error = glGetError(); error; error = glGetError()) {
        LOGI("after %d() glError (0x%x)\n", op, error);
    }
}

JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_load  (JNIEnv *env, jclass bob, jobject callbackClass) {
    GLfloat tex_coords[] = {0,1.0,
                            0,0,
                            4.0,0.0,
                            4.0,1.0};
    GLfloat scanline_coords[] = {0,24.0,
                                 0,0.0,
                                 256.0,0.0,
                                 256.0,24.0};
    jclass cls = env->FindClass("com/tompudding/pipboy/ProgressCallback");
    jmethodID progress_method = NULL;
    jmethodID error_method = NULL;
    if(NULL == cls) {
        LOGI("Error finding class");
    }
    else {
        progress_method = env->GetMethodID(cls, "updateProgress","(F)V");
        if(NULL == progress_method) {
            LOGI("Error finding progress method");
        }
        //error_method = env->GetMethodID(cls, "fatalError", "(java/lang/String;)V");
        //if(NULL == error_method) {
        //    LOGI("Error finding error method");
        //}
    }

    size_t total_items = 30 + numitems; 
    size_t loaded = 0;
    initialised = 1;
    glare           = new Image("screenglare_alpha.png",0.8,1.0,standard_tex_coords);
    fade            = new Image("fade.png",1.0,1.0,standard_tex_coords);
    band            = new Image("band.png",0.8,0.5,tex_coords);
    scanlines       = new Image("scanline.png",0.8,1.0,scanline_coords);
    icon_explosives = new Image("weap_skill_icon_explosives.png",1.0,1.0,standard_tex_coords);
    icon_energy     = new Image("weap_skill_icon_energy.png",1.0,1.0,standard_tex_coords);
    icon_bigguns    = new Image("weap_skill_icon_big_guns.png",1.0,1.0,standard_tex_coords);
    icon_unarmed    = new Image("weap_skill_icon_unarmed.png",1.0,1.0,standard_tex_coords);
    icon_sm_arms    = new Image("weap_skill_icon_sm_arms.png",1.0,1.0,standard_tex_coords);
    icon_melee      = new Image("weap_skill_icon_melee.png",1.0,1.0,standard_tex_coords);
    //arrow           = new Image( "arrow.dimension_64x64.raw",1.0,1.0,standard_tex_coords);
    
    try {
        const char *static_sounds[] = { "ui_static_c_01.wav.snd",
                                        "ui_static_c_02.wav.snd",
                                        "ui_static_c_03.wav.snd",
                                        "ui_static_c_04.wav.snd",
                                        "ui_static_c_05.wav.snd",
                                        "ui_static_d_01.wav.snd",
                                        "ui_static_d_02.wav.snd",
                                        "ui_static_d_03.wav.snd",
                                        "ui_static_d_04.wav.snd",
                                        "ui_static_d_05.wav.snd",
                                        NULL};
        //if(NULL == listener)
        //    listener  = new Listener();
        font      = new Font("monofonto_verylarge02_dialogs2.png","monofonto_verylarge02_dialogs2.fnt");
        general_text = new Text("balls",font);
        //do 12 random weapons

        //memory_leak!
        item_map.clear();
        for(int i=0;i<numitems;i++){
            //Item *x = new Item(itemdata+i,font);
            //LOGI("Adding item %d(%p) to %d : %d,%d",i,x,itemdata[i].code,numweap,numapp);
            item_map[itemdata[i].code] = &(itemdata[i]);
        }
        
        viewlist              = new ViewList();
        mode_change           = new SoundClip( "mode_change.snd");
        menu_tab              = new SoundClip( "menu_tab.snd");
        mode_change_buzz      = new RandomSoundClip((char **)static_sounds);
        menu_prevnext         = new SoundClip( "menu_prevnext.snd");
        equip_weapon_sound    = new SoundClip( "equip_weapon.snd");
        unequip_apparel_sound = new SoundClip( "unequip_apparel.snd");
        unequip_weapon_sound  = new SoundClip( "unequip_weapon.snd");
        equip_apparel_sound   = new SoundClip( "equip_apparel.snd");
        equip_aid_sound       = new SoundClip( "equip_aid.snd");
        equip_misc_sound      = new SoundClip( "equip_misc.snd");
        select_sound          = new SoundClip( "select_sound.snd");
        general_condition_bar = new ItemConditionBar(0.6);

        total_items = NumImages() + NumSounds();
        loaded = 0;

        LoadImages(env,callbackClass,progress_method,&loaded,total_items);
        LoadSounds(env,callbackClass,progress_method,&loaded,total_items);
    }
    catch(error e) {
        LOGI("Error creating  %d",e);
        font = NULL;
    }

}

JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_init  (JNIEnv *env, jclass bob) {
    uint32_t white = 0xffffffff;
    uint32_t grey = 0x808080ff;
    LOGI("init monkey");
    srand48(time(NULL));
    sleep(1);
    glEnable(GL_BLEND);checkGlError(__LINE__);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);checkGlError(__LINE__);
    glEnable(GL_TEXTURE_2D);checkGlError(__LINE__);
    glEnableClientState(GL_VERTEX_ARRAY);checkGlError(__LINE__);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);checkGlError(__LINE__);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);checkGlError(__LINE__);
    glDisable(GL_DEPTH_TEST);checkGlError(__LINE__);
    glColor4f(0.54f, 0.43f, 0.19f,1.0f);
    //glColor4f(0.54f,0.855f,0.58f,1.0f);

    white_texture = GenTexture(1,1,(uint8_t*)&white);
    grey_texture = GenTexture(1,1,(uint8_t*)&grey);
    RefreshImages();
    
    LOGI("init monkey: %p %p",glare,viewlist);
}

JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_step (JNIEnv *, jclass) {
    glDisable(GL_DITHER);checkGlError(__LINE__);

    glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
                 GL_MODULATE);checkGlError(__LINE__);

    /*
     * Usually, the first thing one might want to do is to clear
     * the screen. The most efficient way of doing this is to use
     * glClear().
     */

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);checkGlError(__LINE__);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);checkGlError(__LINE__);

    /*
     * Now we're ready to draw some 3D objects
     */

    glMatrixMode(GL_MODELVIEW);checkGlError(__LINE__);
    glLoadIdentity();checkGlError(__LINE__);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    if(0 != gettimeofday(&tv,NULL)) {
        LOGI("Gettimeofday failed");
    }

    uint32_t milliseconds = ((tv.tv_sec*1000) + (tv.tv_usec/1000));
    GLfloat pos = ((((GLfloat)(milliseconds%4000UL))/4000UL)*1.6)-0.4;
    //LOGI("pos %u %u %u %u %.4f",tv.tv_sec,tv.tv_usec,milliseconds,(milliseconds%4000UL),pos);

    //font->Write("General",0.1,0.5,5.0);
    //stats->Draw(0.1,0);
    viewlist->CurrentView()->Draw(0.1,0);
    band->Draw(0.0,0.8-pos,12.5,1);
    scanlines->Draw(0.1,0);
    glare->Draw(0.1,0);
}

GLuint GenTexture(uint32_t w,uint32_t h,uint8_t *data) {
    GLuint out;
    glGenTextures(1, &out);
    glBindTexture(GL_TEXTURE_2D, out);checkGlError(__LINE__);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);checkGlError(__LINE__);
    glTexParameterf(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR);checkGlError(__LINE__);

    glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);checkGlError(__LINE__);
    glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_EDGE);checkGlError(__LINE__);

    glTexEnvf(GL_TEXTURE_ENV, 
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);checkGlError(__LINE__);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);checkGlError(__LINE__);
    return out;
}

JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_TouchEvent  (JNIEnv *, jclass, jfloat _x, jfloat _y) {
    float x = _x;
    float y = _y;
    LOGI("Native Touch event! %.2f %.2f",x,y);

    //right triangle
    if(y < x && y > 1-x) {
        //if(x > 0.8) {
        //    viewlist->Next();
        //}
        //else {
            viewlist->RightInView();
            //}
    }

    //left triangle
    if(y > x && y < 1-x) {
        //if(x < 0.3) {
        //    viewlist->Prev();
        //}
        //else {
            viewlist->LeftInView();
            //}
    }

    //bottom triangle
    if(y < x && y < 1-x) {
        viewlist->DownInView();
    }

    //top triangle
    if(y > x && y > 1-x) {
        viewlist->UpInView();
    }

    
}

JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_LeftSwipe (JNIEnv *, jclass) {
    LOGI("native leftswipe");
    viewlist->Prev();
}

JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_RightSwipe (JNIEnv *, jclass) {
    LOGI("native rightswipe");
    viewlist->Next();
}

JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_LongPress (JNIEnv *, jclass) {
    LOGI("native longpress");
    viewlist->Select();
}


JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_MenuButton  (JNIEnv *, jclass) {
    LOGI("m1");
    viewlist->Select();
}
