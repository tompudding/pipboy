#include "gl_code.h"
#include <unistd.h>

DataView::DataView (const char *background_filename, Font *_font) {
    struct tm stm;
    time_t t;
    char temp[128];
    pthread_mutex_init(&items_mutex,NULL);
    font = _font;

    lists[0] = new ItemList(&equipped);
    lists[1] = new ItemList(&equipped);
    current_list = 0;

    boxes[0] = new PlacementInfo(0.29,0.005,1,1, new Box(0.210,0.08,0.007));
    boxes[1] = new PlacementInfo(0.293,0.005,1,1, new Box(0.205,0.08,0.007));

    waves[0] = left     = new Image(DATA_DIR     "left.dimensions_256x256.raw",0.63*(480./800),0.482,standard_tex_coords);
    waves[1] = forward  = new Image(DATA_DIR  "forward.dimensions_256x256.raw",0.63*(480./800),0.482,standard_tex_coords);
    waves[2] = backward = new Image(DATA_DIR "backward.dimensions_256x256.raw",0.63*(480./800),0.482,standard_tex_coords);
    waves[3] = wave     = new Image(DATA_DIR    "wave.dimensions_1024x256.raw",0.63*(480./800),0.482,standard_tex_coords);

    /*ItemCode panel_codes[] = {PANEL_BATTLESTAR,
                              PANEL_GAMEOFTHRONES ,
                              PANEL_SHOWTIME ,
                              PANEL_DRHORRIBLE ,
                              PANEL_CAPTAINS ,
                              PANEL_VENTUREBROS ,
                              PANEL_THERIVER ,
                              PANEL_WAREHOUSE13 ,
                              PANEL_TRUEBLOOD ,
                              PANEL_TRS ,
                              PANEL_LOCKEANDKEY ,
                              PANEL_COMMUNITY ,
                              PANEL_JOSS ,
                              PANEL_GRIMM ,
                              PANEL_FRINGE ,
                              PANEL_ALCATRAZ ,
                              PANEL_PERSONOFINTEREST ,
                              PANEL_MYTHBUSTERS ,
                              PANEL_MASQ ,
                              PANEL_YOUNGJUSTICE ,
                              };*/
    ItemCode radio_codes[] = {RADIO_BIG_IRON,
                              RADIO_DO_RIGHT,
                              RADIO_AMERICAN_SWING,
                              RADIO_GENERIC_SWING,
                              RADIO_SUNNING,
                              RADIO_JOLLY,
                              RADIO_CIV,
                              RADIO_WAYBACK,
                              RADIO_MIGHTY,
                              RADIO_IDONT,
                              RADIO_GUY,
                              RADIO_BUTCHER
    };
                           

    for(int i=0;i<sizeof(radio_codes)/sizeof(radio_codes[0]);i++) {
        lists[0]->AddItem(ItemFactory(item_map[radio_codes[i]],_font));
    }
    
    
    t = time(NULL);
    strftime(temp,sizeof(temp),"%d.%m.%y, %H:%M",localtime(&t));
    background = new Image(background_filename,0.8,1,standard_tex_coords);
    items.push_back( PlacementInfo(0.187,0.9450,2.,2.,new Text("DATA",font)) );
    items.push_back( PlacementInfo(0.46,0.91,1.4,1.4,new Text("Halloween Judder",font)) );
    time_text = new Text(strdup(temp),font);
    if(time_text) {
        items.push_back( PlacementInfo(0.73,0.91,1.4,1.4,time_text) );
    }
    items.push_back( PlacementInfo(0.32,0.031,1.4,1.4,new Text("Radio",font)) );
    items.push_back( PlacementInfo(0.561,0.031,1.4,1.4,new Text("Quests",font)) );
    items.push_back( PlacementInfo(0.502,0.250,1,1,new Box(0.63,0.01,0.01)) ) ;
    items.push_back( PlacementInfo(0.88,0.250,1,1,new Box(0.01,0.5,0.01)) ) ;
    for(int i=0;i<37;i++)
    {
        float tine_length = (((i+5)%6)==0 ? 0.06 : (((i+1)%2) == 0) ? 0.03 : 0.01);
        items.push_back( PlacementInfo(0.5103+i*0.01,0.26,1,1,new Box(0,(((i+5)%6)==0 ? 0.06 : (((i+1)%2) == 0) ? 0.03 : 0.01),0.002)) ) ;
    }
    for(int i=0;i<35;i++)
    {
        float tine_length = (((i+1)%6)==0 ? 0.06 : (((i+1)%2) == 0) ? 0.03 : 0.01);
        items.push_back( PlacementInfo(0.88 - ((tine_length*480)/800),0.264 + i*0.0138,1,1,new Box(tine_length,0.0,0.002)) ) ;
    }

    not_playing_line = new PlacementInfo(0.502,0.499,1,1,new Box(0.63,0.004,0.002)) ;

    current_item = items.begin();
    srand48(time(NULL));
    last_change = chosen_int = last_time_update = 0;
}


void DataView::Draw(GLfloat x, GLfloat y,GLfloat xscale, GLfloat yscale) {
    background->Draw(x,y,xscale,yscale);
    uint32_t milliseconds = ((tv.tv_sec*1000) + (tv.tv_usec/1000));

    pthread_mutex_lock(&items_mutex);
    for(items_t::iterator i=items.begin(); i != items.end();i++) {
        i->item->Draw(i->x,i->y,i->xscale,i->yscale);
    }
    lists[current_list]->Draw(0,0,1,1);
    int i = current_list;
    boxes[i]->item->Draw(boxes[i]->x,boxes[i]->y,boxes[i]->xscale,boxes[i]->yscale);
    if(milliseconds - last_time_update > 5000) {
        char temp[128];
        time_t t;
        t = time(NULL);
        strftime(temp,sizeof(temp),"%d.%m.%y, %H:%M",localtime(&t));
        free((void *)time_text->text);
        time_text->text = (const char *)strdup(temp);
    }
    if(CurrentlyPlaying()) {
        //draw awesome animation
        
        GLfloat pos; 
        Image *chosen = NULL;
        //chosen_int = (milliseconds%6000)/1500;
        if(milliseconds - last_change > 1500) {
            last_change = milliseconds;
            chosen_int = lrand48()&0x3;
        }
        pos = ((GLfloat)(milliseconds%500UL))/500UL;
        float limit = 1;
        if(chosen_int == 3) {
            limit /= 4;
            pos = ((GLfloat)(milliseconds%2000UL))/2000UL;
        }
        
        chosen = waves[chosen_int];
        
        
        chosen->mTexBuffer[0] = pos;
        chosen->mTexBuffer[2] = pos;
        chosen->mTexBuffer[4] = limit+pos;
        chosen->mTexBuffer[6] = limit+pos;
        chosen->Draw(not_playing_line->x,0.25,not_playing_line->xscale,not_playing_line->yscale);
    }
    else {
        if(not_playing_line) {
            not_playing_line->item->Draw(not_playing_line->x,not_playing_line->y,not_playing_line->xscale,not_playing_line->yscale);
        } 
    }
    pthread_mutex_unlock(&items_mutex);
}

void DataView::Up() {
    pthread_mutex_lock(&items_mutex);
    lists[current_list]->Prev();
    pthread_mutex_unlock(&items_mutex);
}

void DataView::Down() {
    pthread_mutex_lock(&items_mutex);
    lists[current_list]->Next();
    pthread_mutex_unlock(&items_mutex);
}

void DataView::Select() {
    //FIXME: synchronisation
    pthread_mutex_lock(&items_mutex);
    lists[current_list]->Select();
    pthread_mutex_unlock(&items_mutex);
}


RadioItem::RadioItem(const ItemData *data,Font *font) : Item(data,font),box(0.5,0.08,0.007){
    char temp_path[1024] = DATA_DIR;
    name = strdup(data->name);
    code = data->code;
    type = data->type;
    
    strncat(temp_path,data->filename,sizeof(temp_path)-strlen(temp_path));
    LOGI("Trying to open radio sound %s",temp_path);
    clip = new SoundClip(temp_path);
    text = new Text(name,font);
}

