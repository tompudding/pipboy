#ifndef __GL_CODE_H__
#define __GL_CODE_H__
#include <jni.h>
#include <android/log.h>

#include <GLES/gl.h>
#include <GLES/glext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <png.h>
#include <zip.h>
#include <map>
#include <list>
#include <pthread.h>
#include <stdint.h>

#include <assert.h>
#include <jni.h>


#include <SLES/OpenSLES.h>
#include "SLES/OpenSLES_Android.h"

#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "itemcodes.h"


using namespace std;

#define  LOG_TAG    "nativepipboy"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#ifndef _Included_com_tompudding_pipboy_NativePipboy
#define _Included_com_tompudding_pipboy_NativePipboy
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_tompudding_pipboy_NativePipboy
 * Method:    init
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_load (JNIEnv *, jclass, jobject callbackClass);
JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_init (JNIEnv *, jclass);

/*
 * Class:     com_tompudding_pipboy_NativePipboy
 * Method:    step
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_step
  (JNIEnv *, jclass);

    JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_TouchEvent (JNIEnv *, jclass, jfloat x, jfloat y);

JNIEXPORT jboolean JNICALL Java_com_tompudding_pipboy_NativePipboy_selectClip(JNIEnv* env, jclass clazz, jint which,jint count);

    JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_MenuButton (JNIEnv *, jclass);
    JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_LeftSwipe (JNIEnv *, jclass);
    JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_RightSwipe (JNIEnv *, jclass);
    JNIEXPORT void JNICALL Java_com_tompudding_pipboy_NativePipboy_LongPress (JNIEnv *, jclass);
    

    int GetExit();
    void SetExit(int);


#ifdef __cplusplus
    }
#endif
#endif

enum error {
    OK,
    FILE_NOT_FOUND,
    FILE_ERROR,
    BIND_ERROR,
    LISTEN_ERROR,
    SOCKET_ERROR,
    THREAD_ERROR,
    UNINITIALISED,
    MEMORY_ERROR
};

enum message_types {
    next_item     = 1,
    horizontal    = 2,
    vertical      = 3,
    view_right    = 4,
    view_left     = 5,
    view_absolute = 6,
    view_rotary   = 7,
    select_but    = 8,
    vertical_pos  = 9
};

struct Message {
    uint32_t type;
    int32_t data[4];
};

extern "C" {void PlayClip_c(short *buffer,size_t size,int queue,int q);}
extern "C" {uint32_t CurrentlyPlaying();}
extern "C" {void StopClip();}
GLuint GenTexture(uint32_t w,uint32_t h,uint8_t *data);

class Listener {
public:
    Listener(int port=4919);
    ~Listener();

    pthread_t listen_thread;
};

class SoundClip {
public:
    SoundClip() {};
    SoundClip(const char *filename);
    ~SoundClip();

    virtual void GetBuffer(short **output,size_t *size);
    virtual void Load();
    virtual size_t Size();

    short *buffer;
    size_t size;
    char *fname;
    bool loaded;
};

class RandomSoundClip : public SoundClip{
public:
    RandomSoundClip(char **filename);
    ~RandomSoundClip();

    virtual void GetBuffer(short **output,size_t *size);
    virtual void Load();
    virtual size_t Size();

    short **buffer;
    char **filenames;
    size_t *size;
    size_t count;
    size_t total_size;
};

extern struct timeval tv;

class Drawable {
public:
    Drawable() {};
    virtual void Draw(GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) = 0;
};

class Image : public Drawable {
public:
    Image(const char *filename, GLfloat width, GLfloat height, GLfloat *tex_coords);
    ~Image();

    void    Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void RefreshTexture();
    void Load();
    size_t Size();

    GLuint  texture;
    GLfloat mFVertexBuffer[4*3];
    GLfloat mTexBuffer    [4*2];
    GLfloat mIndexBuffer  [4];
    GLfloat width,height;
    uint8_t *data;
    uint32_t file_width;
    uint32_t file_height;
    size_t size;
    bool loaded;
    char *fname;
};

class ImagePtr : public Drawable {
public:
    ImagePtr(Image *orig, GLfloat width, GLfloat height, GLfloat *tex_coords);
    void    Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    
    GLfloat mFVertexBuffer[4*3];
    GLfloat mTexBuffer    [4*2];
    GLfloat mIndexBuffer  [4];
    GLfloat width,height;
    Image *imageptr;
};

void RefreshImages(void);
void LoadImages(JNIEnv *env, jobject callbackClass, jmethodID progress_method, size_t *loaded, size_t total_items);
void LoadSounds(JNIEnv *env, jobject callbackClass, jmethodID progress_method, size_t *loaded, size_t total_items);
size_t NumImages();
size_t NumSounds();

class Box : public Drawable {
public:
    Box(GLfloat _width,GLfloat _height, GLfloat _thickness);

    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);

    GLfloat width;
    GLfloat height;
    GLfloat thickness;

    GLfloat mFVertexBuffer[4*3*4];
    GLfloat mTexBuffer    [4*2];
    GLfloat mIndexBuffer  [4];
};

class HalfBox : public Drawable {
public:
    HalfBox(GLfloat _width,GLfloat _height, GLfloat _thickness);

    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void SetSize(GLfloat _width,GLfloat _height,GLfloat _thickness);

    GLfloat width;
    GLfloat height;
    GLfloat thickness;

    GLfloat mFVertexBuffer[4*3*2];
    GLfloat mTexBuffer    [4*2];
    GLfloat mIndexBuffer  [4];
};

class ItemConditionBar : public Drawable {
public:
    ItemConditionBar(GLfloat value);

    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void SetSize(GLfloat value);

    GLfloat width;
    GLfloat height;
    GLfloat value;

    GLfloat mFVertexBuffer[4*3*2];
    GLfloat mTexBuffer    [4*2];
    GLfloat mIndexBuffer  [4];
};



enum ConditionBarType {
    cb_plain,
    cb_pointleft,
    cb_pointright
};

class ScrollBar : public Drawable {
public:
    ScrollBar();
    ~ScrollBar();

    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void SetData(uint32_t total_items,uint32_t current_item, uint32_t window_size);

    uint32_t total_items;
    uint32_t current_item;
    uint32_t window_size;

    Image *top;
    Image *bottom;
    Image *bar;
};



class ConditionBar : public Drawable {
public:
    ConditionBar(GLfloat _value, ConditionBarType _type);

    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);

    GLfloat mFVertexBuffer[4*3*4];
    GLfloat mFTriangleBuffer[3*3*4];
    GLfloat mTexBuffer    [4*2];
    GLfloat mIndexBuffer  [4];
    GLfloat height;
    GLfloat width;
    GLfloat value;
    GLfloat thickness;
    ConditionBarType type;
};

typedef map<char,ImagePtr*> letters_t;
typedef map<char,GLfloat> offsets_t;

class Font {
public:
    Font(const char *texture_filename, const char *fnt_filename);

    void Write(const char *string,GLfloat x, GLfloat y, GLfloat scale);

    letters_t letters;
    offsets_t offsets;
    Image   *texture;
};

class Text : public Drawable {
public:
    Text(const char *_text, Font *_font) : text(_text),font(_font) {};
    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);

    const char *text;
    Font *font;
};

class Character : public Drawable {
public:
    Character();

    Image *heads;
    Image *faces;
    Image *left_arms;
    Image *right_arms;
    Image *left_legs;
    Image *right_legs;
    Image *torsos;

    Text *title;

    void  Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
};

class PlacementInfo {
public:
PlacementInfo(GLfloat _x, \
              GLfloat _y, \
              GLfloat _xscale, \
              GLfloat _yscale, \
              Drawable *_item) : x(_x),y(_y),xscale(_xscale),yscale(_yscale),item(_item) {};

    GLfloat x;
    GLfloat y;
    GLfloat xscale;
    GLfloat yscale;
    Drawable *item;
};

typedef list<PlacementInfo> items_t;

struct Requirements {
    int32_t strength;
    int32_t unarmed;
    int32_t small_arms;
    int32_t melee;
    int32_t explosives;
    int32_t energy;
    int32_t big_guns;
};

struct ItemData {
    ItemType type;
    ItemCode code;
    float weight;
    float damage;
    Requirements requirements;
    uint32_t value;
    int32_t damage_reduction;
    float condition;
    uint32_t calibre;
    
    const char *armour_class;
    const char *special;
    const char *name;
    const char *filename;
};

struct EquippedItems; //damn, I made a forward declaration necessary

class Item : public Drawable {
public:
    Item(const char *_name,const char *filename, Font *font);
    Item(const ItemData *data,Font *font);
    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    virtual void Select(EquippedItems *equipped) {};

    float weight;
    float damage;
    Requirements requirements;
    uint32_t value;
    int32_t damage_reduction;
    float condition;
    uint32_t calibre;
    ItemType type;
    ItemCode code;
    
    const char *armour_class;
    const char *special;
    char *name;

    Image *icon;
    Text *text;
};

class WeaponItem : public Item {
public:
WeaponItem(const ItemData *data,Font *font) : Item(data,font),box(0.5,0.08,0.007) {};
    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void Select(EquippedItems *equipped);
    
    HalfBox box;
};

class ApparelItem : public Item {
public:
    ApparelItem(const ItemData *data,Font *font) : Item(data,font),box(0.5,0.08,0.007) {};
    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void Select(EquippedItems *equipped);

    HalfBox box;
};

class AidItem : public Item {
public:
    AidItem(const ItemData *data,Font *font) : Item(data,font),box(0.5,0.08,0.007) {};
    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void Select(EquippedItems *equipped);

    HalfBox box;
};

class MiscItem : public Item {
public:
    MiscItem(const ItemData *data,Font *font) : Item(data,font),box(0.5,0.08,0.007) {};
    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void Select(EquippedItems *equipped);

    HalfBox box;
};

class AmmoItem : public Item {
public:
    AmmoItem(const ItemData *data,Font *font) : Item(data,font),box(0.5,0.08,0.007) {};
    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);

    HalfBox box;
};

class PanelItem : public Item {
public:
    PanelItem(const ItemData *data,Font *font) : Item(data,font),box(0.5,0.08,0.007) {};
    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);

    HalfBox box;
};

class RadioItem : public Item {
public:
    RadioItem(const ItemData *data,Font *font);
    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void Select(EquippedItems *equipped);

    HalfBox box;
    SoundClip *clip;
};

struct EquippedItems {
EquippedItems() : weapon(NO_ITEM),apparel_legs(NO_ITEM),apparel_hat(NO_ITEM),apparel_glasses(NO_ITEM) {};
    ItemCode weapon;
    ItemCode apparel_legs;
    ItemCode apparel_hat;
    ItemCode apparel_glasses;
};

typedef list<Item*> itemlist_t;

class ItemList : public Drawable {
public:
    ItemList(EquippedItems *a);
    ~ItemList();

    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void AddItem(Item *item);
    void Next();
    void Prev();
    void Select();
    void VerticalPos(float pos,int sound);

    itemlist_t itemlist;
    itemlist_t::iterator current_item;
    itemlist_t::iterator equipped_item;
    size_t display_start;
    size_t current_pos;
    size_t display_num;
    
    Box selected_box;
    Box selected_boxnub;
    Image *equipped_box;
    ScrollBar *scrollbar;
    EquippedItems *equipped;
};



class View {
public:
    View() {};
    virtual void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);

    void NextItem();
    void Move(GLfloat x, GLfloat y);

    virtual void Up()    {};
    virtual void Down()  {};
    virtual void Left()  {};
    virtual void Right() {};
    virtual void Select() {};
    virtual void SetRotary(uint32_t value,float pos,uint32_t sound) {};
    virtual void VerticalPos(float pos,int sound) {};

    items_t items;
    items_t::iterator current_item;
    pthread_mutex_t items_mutex;
};

class ConditionSubView : public View {
public:
    ConditionSubView() ;
    
};

class RadiationSubView : public View {
public:
    RadiationSubView() ; 
};

class EffectsSubView : public View {
public:
    EffectsSubView() ; 
};

class WaterSubView : public View {
public:
    WaterSubView() ; 
};

class FoodSubView : public View {
public:
    FoodSubView() ; 
};

class SleepSubView : public View {
public:
    SleepSubView() ; 
};


class StatusSubView : public View {
public:
    StatusSubView();

    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void Up() ;
    void Down() ;
    void VerticalPos(float pos,int sound);

    View *subviews[6];
    uint32_t current_view;
    PlacementInfo *boxes[6];
};

class SpecialSubView : public View {
public:
    SpecialSubView();
    
    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void Up() ;
    void Down() ;
    void VerticalPos(float pos,int sound);

    uint32_t stats[7];
    Image *icons[7];
    const char *names[7]; 
    const char *descriptions[7][5];
    uint32_t current_item;
    Box selected_box;
    HalfBox box;
};

class SkillsSubView : public View {
public:
    SkillsSubView();

    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void Up() ;
    void Down() ;
    void VerticalPos(float pos,int sound);

    uint32_t stats[13];
    Image *icons[13];
    const char *names[13]; 
    const char *descriptions[13][5];
    uint32_t current_item;
    Box selected_box;
    HalfBox box;
    size_t display_start;
    size_t current_pos;
    size_t display_num;
    ScrollBar *scrollbar;
    
};

class PerksSubView : public View {
public:
    PerksSubView();
    
    void Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void Up() ;
    void Down() ;
    void VerticalPos(float pos,int sound);

    Image *icons[13];
    const char *names[13]; 
    const char *descriptions[13][5];
    uint32_t current_item;
    Box selected_box;
    HalfBox box;
    size_t display_start;
    size_t current_pos;
    size_t display_num;
    ScrollBar *scrollbar;

};

class GeneralSubView : public View {
public:
    GeneralSubView();
    
    void Up() {};
    void Down() {};
    Box selected_box;
    Image *icon;
};


class StatsView : public View {
public:
    StatsView (const char *background_filename, Font *font);
    void    Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);

    void Up()    ;
    void Down()  ;
    void Left()  ;
    void Right() ;
    void Select() ;
    void SetRotary(uint32_t value,float pos,uint32_t sound);
    void VerticalPos(float pos,int sound);

    Font *font;
    Image *background;

    int xp;
    int ap;
    int hp;
    int lvl;
    View *subviews[5];
    uint32_t current_view;
    PlacementInfo *boxes[5];
};

class ItemsView : public View {
public:
    ItemsView (const char *background_filename, Font *font);
    void    Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void Up();
    void Down();
    void Right();
    void Left();
    void Select();
    void SetRotary(uint32_t value,float pos,uint32_t sound);
    void VerticalPos(float pos,int sound);

    Font *font;
    Image *background;
    ItemList *weapons_list;
    ItemList *apparel_list;
    ItemList *aid_list;
    ItemList *misc_list;
    ItemList *ammo_list;
    ItemList *lists[5];
    PlacementInfo *boxes[5];
    size_t current_list;
    EquippedItems equipped;
};

class DataView : public View {
public:
    DataView (const char *background_filename, Font *font);
    void    Draw(GLfloat x,GLfloat y,GLfloat xscale=1.0, GLfloat yscale=1.0);
    void Up();
    void Down();
    void Select();

    Font *font;
    Image *background;
    Image *left,*forward,*backward,*wave;
    Image *waves[4];
    size_t current_list;
    ItemList *panels;
    ItemList *lists[2];
    Text *time_text;
    EquippedItems equipped;
    PlacementInfo *boxes[2];
    PlacementInfo *not_playing_line;
    uint32_t last_change;
    uint32_t last_time_update;
    int chosen_int;
};

typedef list<View*> viewlist_t;

class ViewList {
public:
    ViewList();
    ~ViewList();

    View *CurrentView();
    void Next();
    void Prev();

    void UpInView();
    void LeftInView();
    void DownInView();
    void RightInView();
    void Select();
    void SwitchView(uint32_t view,uint32_t rotary,float pos);
    size_t NumLoadable();

    pthread_mutex_t viewlist_mutex;
    viewlist_t viewlist;
    viewlist_t::iterator current_view;
};


typedef map<ItemCode,ItemData*> itemmap_t;

void PlayClip(SoundClip &sc,int queue = 0,int q=0);


void checkGlError(int op);
Item *ItemFactory(const ItemData *data,Font *font);

extern Image *glare     ;
extern Image *band      ;
extern Image *scanlines ;
extern Image *fade      ;
extern Image *icon_explosives;
extern Image *icon_energy;
extern Image *icon_bigguns;
extern Image *icon_unarmed;
extern Image *icon_sm_arms;
extern Image *icon_melee;
extern Image *arrow;


extern Font *font       ;
extern ViewList *viewlist;
extern Listener *listener;
extern pthread_mutex_t exit_mutex;
extern int exit_status;
extern GLuint white_texture;
extern GLuint grey_texture;

extern SoundClip *mode_change;
extern SoundClip *menu_tab;
extern SoundClip *mode_change_buzz;
extern SoundClip *menu_prevnext;
extern SoundClip *equip_weapon_sound;
extern SoundClip *equip_apparel_sound;
extern SoundClip *equip_aid_sound;
extern SoundClip *equip_misc_sound;
extern SoundClip *unequip_weapon_sound;
extern SoundClip *unequip_apparel_sound;
extern SoundClip *select_sound;
extern Text *general_text;
extern ItemConditionBar *general_condition_bar;

extern GLfloat standard_tex_coords[];

extern ItemData itemdata[];
extern size_t numitems;
extern itemmap_t item_map;


#define DATA_DIR "/storage/sdcard0/pipboy/"
#define ZIP_FILENAME "data.zip"

#endif
