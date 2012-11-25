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



ItemsView::ItemsView (const char *background_filename, Font *_font){
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
    }

    items.push_back( PlacementInfo(0.178,0.9450,2.,2.,new Text("ITEMS",font)) );
    items.push_back( PlacementInfo(0.314,0.91,1.4,1.4,new Text("Wg 168/210",font)) );
    items.push_back( PlacementInfo(0.469,0.91,1.4,1.4,new Text("HP 335/335",font)) );
    items.push_back( PlacementInfo(0.644,0.91,1.4,1.4,new Text("DT 36.0",font)) );
    items.push_back( PlacementInfo(0.771,0.91,1.4,1.4,new Text("Caps 580",font)) );
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
