#include "gl_code.h"
#include <unistd.h>

Item *ItemFactory(const ItemData *data,Font *font) {
    switch(data->type){
    case WEAPON:
        return new WeaponItem(data,font);
    case APPAREL:
        return new ApparelItem(data,font);
    case AID:
        return new AidItem(data,font);
    case MISC:
        return new MiscItem(data,font);
    case AMMO:
        return new AmmoItem(data,font);
    case PANEL:
        return new PanelItem(data,font);
    case RADIO:
        return new RadioItem(data,font);
    }
    return NULL;
}

static ItemType type_from_string(char *input) {
    ItemType types[] = {WEAPON,APPAREL,AID,MISC,AMMO,PANEL,RADIO};
    const char *names[] = {"weapons",
                     "apparel",
                     "aid",
                     "misc",
                     "ammo",
                     "panel",
                     "radio"};
    size_t num_items = sizeof(types)/sizeof(types[0]);
    int i;
    for(i=0;i<num_items;i++) {
        if(strcasecmp(input,names[i]) == 0) {
            return types[i];
        }
    }
    return (ItemType)-1;
}

ItemsView::ItemsView (const char *background_filename, Font *_font, GeneralConfig &config){
    FILE *f = NULL;
    char buffer[1024] = {0};
    int i;
    int code = 0;
    pthread_mutex_init(&items_mutex,NULL);
    background = new Image(background_filename,0.8,1,standard_tex_coords);
    font = _font;

    lists[0] = weapons_list = new ItemList(&equipped);
    lists[1] = apparel_list = new ItemList(&equipped);
    lists[2] = aid_list     = new ItemList(&equipped);
    lists[3] = misc_list    = new ItemList(&equipped);
    lists[4] = ammo_list    = new ItemList(&equipped);
    current_list = 0;

    boxes[0] = new PlacementInfo(0.14,0.005,1,1, new Box(0.195,0.08,0.007));
    boxes[1] = new PlacementInfo(0.293,0.005,1,1, new Box(0.205,0.08,0.007));
    boxes[2] = new PlacementInfo(0.466,0.005,1,1, new Box(0.11,0.08,0.007));
    boxes[3] = new PlacementInfo(0.576,0.005,1,1, new Box(0.125,0.08,0.007));
    boxes[4] = new PlacementInfo(0.725,0.005,1,1, new Box(0.155,0.08,0.007));

    f = fopen(DATA_DIR "items.txt","rb");
    if(NULL == f) {
        string message("Error opening items datafile ");
        message += DATA_DIR "items.txt";
        //Memory leak!
        throw ErrorMessage(FILE_ERROR,message);
    }
    while(NULL != fgets(buffer,sizeof(buffer),f)) {
        if(buffer[0] == '\0' || buffer[0] == '#' || buffer[0] == '\n') {
            continue;
        }
        
        char *data[18] = {0};
        char *saveptr = NULL;
        ItemData itemdata = {(ItemType)0};
        data[0] = strtok_r(buffer,"|",&saveptr);
        if(data[0] == NULL) {
            continue;
        }
        for(i=1;i<18;i++) {
            data[i] = strtok_r(NULL,"|",&saveptr);
            if(data[i] == NULL) {
                break;
            }
        }
        if(i != 18) {
            continue;
        }
        //remove the newline from the end of the last item
        i = strlen(data[17]) - 1;
        while(i > 0 && (data[17][i] == ' '  || 
                        data[17][i] == '\n' || 
                        data[17][i] == '\r' || 
                        data[17][i] == '\t')) {
            data[17][i] = '\0';
            i--;
        }

        itemdata.type                    = type_from_string(data[0]);
        itemdata.code                    = (ItemCode)code++; //sort of cheat here
        itemdata.weight                  = strtof(data[ 2],NULL);
        itemdata.damage                  = strtof(data[ 3],NULL);
        itemdata.requirements.strength   = strtol(data[ 4],NULL,10);
        itemdata.requirements.unarmed    = strtol(data[ 5],NULL,10);
        itemdata.requirements.small_arms = strtol(data[ 6],NULL,10);
        itemdata.requirements.melee      = strtol(data[ 7],NULL,10);
        itemdata.requirements.explosives = strtol(data[ 8],NULL,10);
        itemdata.requirements.energy     = strtol(data[ 9],NULL,10);
        itemdata.requirements.big_guns   = strtol(data[10],NULL,10);
        itemdata.value                   = strtoul(data[11],NULL,10);
        itemdata.damage_reduction        = strtol(data[12],NULL,10);
        itemdata.condition               = strtof(data[13],NULL);
        itemdata.calibre                = strtof(data[14],NULL);
        itemdata.armour_class            = strdup(data[15]);
        itemdata.special                 = strdup(data[16]);
        itemdata.name                    = strdup(data[1]);
        itemdata.filename                = strdup(data[17]);

        switch(itemdata.type){
        case WEAPON:
            weapons_list->AddItem(new WeaponItem(&itemdata,_font));
            break;
        case APPAREL:
            apparel_list->AddItem(new ApparelItem(&itemdata,_font));
            break;
        case AID:
            aid_list->AddItem(new AidItem(&itemdata,_font));
            break;
        case MISC:
            misc_list->AddItem(new MiscItem(&itemdata,_font));
            break;
        case AMMO:
            ammo_list->AddItem(new AmmoItem(&itemdata,_font));
            break;
       }

    }
/*
    ItemCode weapons[] = {WEAPONS_POOL_CUE,
                          WEAPONS_TIRE_IRON,
                          WEAPONS_ALIEN_BLASTER};
    ItemCode apparels[] = {VAULT_SUIT_76,
                           APPERAL_EYEGLASSES,
                           APPERAL_SHADES,
                           APPERAL_RED_BANDANA};
    ItemCode aids[]     = {ITEM_ANTIVENOM,
                           ITEM_BUFFOUT,
                           ITEM_CATEYE,
                           ITEM_COYOTE_TOBACCO,
                           ITEMS_WATER,
                           ITEM_DOCTORS_BAG,
                           ITEMS_SKILL_MAGS_REPAIR,
                           ITEMS_SKILL_MAGS_ENERGY_WEAPONS,
                           ITEMS_IGUANA_STICK,
                           ITEM_JET,
                           ITEMS_MENTATS,
                           ITEMS_FRESH_MAIZE,
                           ITEMS_NOODLES};
    ItemCode misc[]     = {ITEM_BOBBY_PIN,
                           ITEM_CARAVAN_DECK,
                           ITEMS_FOOD_ADDITIVE,
                           ITEMS_HOLO_DOGTAG,
                           ITEM_MARK_OF_CAESAR,
                           ITEM_KEYRING};
    ItemCode ammo[]     = {ITEMS__308_AMMO};
                           

    for(int i=0;i<sizeof(weapons)/sizeof(weapons[0]);i++) {
        weapons_list->AddItem(ItemFactory(item_map[weapons[i]],_font));
    }
    for(int i=0;i<sizeof(apparels)/sizeof(apparels[0]);i++) {
        apparel_list->AddItem(ItemFactory(item_map[apparels[i]],_font));
    }
    for(int i=0;i<sizeof(aids)/sizeof(aids[0]);i++) {
        aid_list->AddItem(ItemFactory(item_map[aids[i]],_font));
    }
    for(int i=0;i<sizeof(misc)/sizeof(misc[0]);i++) {
       misc_list->AddItem(ItemFactory(item_map[misc[i]],_font));
    }
    for(int i=0;i<sizeof(ammo)/sizeof(ammo[0]);i++) {
       ammo_list->AddItem(ItemFactory(item_map[ammo[i]],_font));
       }*/

    items.push_back( PlacementInfo(0.178,0.9450,2.,2.,new Text("ITEMS",font)) );
    items.push_back( PlacementInfo(0.314,0.91,1.4,1.4,new Text("Wg 168/210",font)) );
    if(sizeof(buffer) <= snprintf(buffer,sizeof(buffer),"HP %d/%d",config.hp.current,config.hp.max)) {
        throw ErrorMessage(MEMORY_ERROR,"snprintf error in ItemsView::ItemsView");
    }
    buffer[sizeof(buffer)-1] = 0;
    items.push_back( PlacementInfo(0.469,0.91,1.4,1.4,new Text(buffer,font)) );
    items.push_back( PlacementInfo(0.644,0.91,1.4,1.4,new Text("DT 36.0",font)) );
    if(sizeof(buffer) <= snprintf(buffer,sizeof(buffer),"Caps %d",config.caps)) {
        throw ErrorMessage(MEMORY_ERROR,"snprintf error in ItemsView::ItemsView");
    }
    buffer[sizeof(buffer)-1] = 0;
    items.push_back( PlacementInfo(0.771,0.91,1.4,1.4,new Text(buffer,font)) );
    items.push_back( PlacementInfo(0.152,0.031,1.4,1.4,new Text("Weapons",font)) );
    items.push_back( PlacementInfo(0.306,0.031,1.4,1.4,new Text("Apparel",font)) );
    items.push_back( PlacementInfo(0.48,0.031,1.4,1.4,new Text("Aid",font)) );
    items.push_back( PlacementInfo(0.592,0.031,1.4,1.4,new Text("Misc",font)) );
    items.push_back( PlacementInfo(0.742,0.031,1.4,1.4,new Text("Ammo",font)) );
    //items.push_back( PlacementInfo(0.25,0.005,1,1, new Box(0.195,0.08,0.007)));
    //items.push_back( PlacementInfo(0,0,1,1,item_list) );
    current_item = items.begin();
}

void ItemsView::Up() {
    lists[current_list]->Prev();
}

void ItemsView::Down() {
    lists[current_list]->Next();
}



void ItemsView::Right() {
    //FIXME: synchronisation
    pthread_mutex_lock(&items_mutex);
    PlayClip(*menu_tab);
    current_list = (current_list + 1)%5;
    pthread_mutex_unlock(&items_mutex);
}

void ItemsView::SetRotary(uint32_t value,float pos,uint32_t sound) {
    pthread_mutex_lock(&items_mutex);
    if(current_list != value) {
        if(sound)
            PlayClip(*menu_tab);
        current_list = value%5;
    }
    lists[current_list]->VerticalPos(pos,0);
    pthread_mutex_unlock(&items_mutex);
}

void ItemsView::VerticalPos(float pos,int sound)
{
    lists[current_list]->VerticalPos(pos,sound);
}

void ItemsView::Left() {
    //FIXME: synchronisation
    pthread_mutex_lock(&items_mutex);
    PlayClip(*menu_tab);
    current_list = (current_list + 4)%5;
    pthread_mutex_unlock(&items_mutex);
}

void ItemsView::Select() {
    //FIXME: synchronisation
    pthread_mutex_lock(&items_mutex);
    lists[current_list]->Select();
    pthread_mutex_unlock(&items_mutex);
}

void ItemsView::Draw(GLfloat x, GLfloat y,GLfloat xscale, GLfloat yscale) {
    background->Draw(x,y,xscale,yscale);

    pthread_mutex_lock(&items_mutex);
    for(items_t::iterator i=items.begin(); i != items.end();i++) {
        i->item->Draw(i->x,i->y,i->xscale,i->yscale);
    }
    //draw the current itemlist
    lists[current_list]->Draw(0,0,1,1);
    int i = current_list;
    boxes[i]->item->Draw(boxes[i]->x,boxes[i]->y,boxes[i]->xscale,boxes[i]->yscale);
    pthread_mutex_unlock(&items_mutex);
}
