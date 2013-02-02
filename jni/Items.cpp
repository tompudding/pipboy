#include "gl_code.h"
#include <unistd.h>

static SoundClip *last_played = NULL;

ItemList::ItemList(EquippedItems *a) : selected_box(0.52,0.08,0.007),selected_boxnub(0.04,0.04,0.006),equipped(a) {
    equipped_item = current_item = itemlist.end();
    current_pos = display_start = 0;
    display_num = 8;
    scrollbar = new ScrollBar();
    scrollbar->SetData(itemlist.size(),display_start,display_num);
    equipped_box = new Image("full.png",480.0/800,1,standard_tex_coords);
}

ItemList::~ItemList() {
    for(itemlist_t::iterator i = itemlist.begin(); i != itemlist.end(); i++) {
        delete *i;
    }
}

void ItemList::AddItem(Item *item) {
    if(item == NULL)
        return;
    itemlist.push_back(item);
    if(current_item == itemlist.end()) {
        current_item  = itemlist.begin();
    }
    scrollbar->SetData(itemlist.size(),display_start,display_num);
}

Item::Item(const char *_name,const char *filename, Font *font) {
    icon = new Image(filename,1.0*480/800,1.0,standard_tex_coords);
    name = strdup(_name); //should free this in the destructor really
    text = new Text(name,font);
}

Item::Item(const ItemData *data,Font *font){
    char temp_path[1024] = "icons/";
    weight = data->weight;
    damage = data->damage;
    requirements = data->requirements;
    value = data->value;
    condition = data->condition;
    damage_reduction = data->damage_reduction;
    calibre = data->condition;
    armour_class = data->armour_class;
    special = data->special;
    name = strdup(data->name);
    code = data->code;
    type = data->type;
    
    if(data->type != RADIO && data->type != EXIT) {
        strncat(temp_path,data->filename,sizeof(temp_path) - strlen(temp_path));
        LOGI("Trying to open %s",temp_path);
        icon = new Image(temp_path,1.0*480/800,1.0,standard_tex_coords);
    }
    else {
        icon = NULL;
    }
    text = new Text(name,font);
    LOGI("Item constructor %s %p %p",name,icon,text);
}

void Item::Draw(GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    icon->Draw(x+0.492,y+0.33,0.65,0.65);
}

void WeaponItem::Draw(GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    char temp[512];
    GLfloat w = 0.224;
    Item::Draw(x,y,xscale,yscale);
    box.SetSize(w,0.08,0.007);
    box.Draw(x+0.45,y+0.35);
    box.Draw(x+0.465+w*480/800,y+0.35);
    box.Draw(x+0.48+2*w*480/800,y+0.35);
    box.Draw(x+0.48+2*w*480/800,y+0.44);
    box.Draw(x+0.45,y+0.26);
    box.SetSize(w*2+(0.015*800/480),0.08,0.007);
    box.Draw(x+0.465+w*480/800,y+0.26);

    snprintf(temp,sizeof(temp),"DAM %4.1f",damage);
    general_text->SetText((const char*)&temp);
    general_text->Draw(x+0.45+0.005,y+0.35+0.02,1.5,1.5);

    snprintf(temp,sizeof(temp),"WG  %4.1f",weight);
    general_text->SetText( (const char*)&temp);
    general_text->Draw(x+0.465+w*480/800+0.005,y+0.35+0.02,1.6,1.6);

    snprintf(temp,sizeof(temp),"VAL %4d",value);
    general_text->SetText( (const char*)&temp);
    general_text->Draw(x+0.48+2*w*480/800+0.005,y+0.35+0.02,1.6,1.6);

    snprintf(temp,sizeof(temp),"STR %3d",requirements.strength);
    general_text->SetText( (const char*)&temp);
    general_text->Draw(x+0.48+2*w*480/800+0.005,y+0.44+0.02,1.6,1.6);

    if(calibre != 0)
    {
        snprintf(temp,sizeof(temp),".%3d (6/120)",calibre);
        general_text->SetText( (const char*)&temp);
    }
    else
        general_text->SetText( "--");
    general_text->Draw(x+0.465+w*480/800+0.005,y+0.26+0.02,1.6,1.6);

    general_text->SetText( "CND");
    general_text->Draw(x+0.45+0.005,y+0.26+0.02,1.5,1.5);

    general_condition_bar->SetSize(condition);
    general_condition_bar->Draw(x+0.5+0.005,y+0.26+0.02,1,1);

    Image *todraw = NULL;
    int todrawv = 0;
    if(requirements.unarmed >= 0)
    {
        todraw = icon_energy;
        todrawv = requirements.unarmed;
    }
    else if(requirements.small_arms >= 0)
    {
        todraw = icon_sm_arms;
        todrawv = requirements.small_arms;
    }
    else if(requirements.melee >= 0)
    {
        todraw = icon_melee;
        todrawv = requirements.melee;
    }
    else if(requirements.explosives >= 0)
    {
        todraw = icon_explosives;
        todrawv = requirements.explosives;
    }
    else if(requirements.energy >= 0)
    {
        todraw = icon_energy;
        todrawv = requirements.energy;
    }
    else if(requirements.big_guns >= 0)
    {
        todraw = icon_bigguns;
        todrawv = requirements.big_guns;
    }
    if(todraw){
        todraw->Draw(x+0.45,y+0.44+0.02,0.1*480/800,0.1);
        snprintf(temp,sizeof(temp),"%2d",todrawv);
        general_text->SetText( (const char*)&temp);
        general_text->Draw(x+0.50,y+0.44+0.02,1.6,1.6);
    }
    //
    
}

void ApparelItem::Draw(GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    char temp[512];
    GLfloat w = 0.224;
    Item::Draw(x,y,xscale,yscale);
    box.SetSize(w,0.08,0.007);
    box.Draw(x+0.45,y+0.35);
    box.Draw(x+0.465+w*480/800,y+0.35);
    box.Draw(x+0.48+2*w*480/800,y+0.35);
    
    box.Draw(x+0.45,y+0.26);
    box.SetSize(w*2+(0.015*800/480),0.08,0.007);
    box.Draw(x+0.465+w*480/800,y+0.26);
    box.SetSize(w*3+(0.03*800/480),0.08,0.007);
    box.Draw(x+0.45,y+0.17);

    if(damage_reduction >= 0) {
        snprintf(temp,sizeof(temp),"DR %4d",damage_reduction);
        general_text->SetText( (const char*)&temp);
    }
    else
        general_text->SetText( "--");
    general_text->Draw(x+0.45+0.005,y+0.35+0.02,1.5,1.5);

    snprintf(temp,sizeof(temp),"WG  %4.1f",weight);
    general_text->SetText( (const char*)&temp);
    general_text->Draw(x+0.465+w*480/800+0.005,y+0.35+0.02,1.6,1.6);

    snprintf(temp,sizeof(temp),"VAL %4d",value);
    general_text->SetText( (const char*)&temp);
    general_text->Draw(x+0.48+2*w*480/800+0.005,y+0.35+0.02,1.6,1.6);
 
    general_text->SetText( armour_class);   
    general_text->Draw(x+0.465+w*480/800+0.005,y+0.26+0.02,1.6,1.6);

    general_text->SetText( special);   
    general_text->Draw(x+0.45,y+0.17+0.02,1.6,1.6);

    general_text->SetText( "CND");
    general_text->Draw(x+0.45+0.005,y+0.26+0.02,1.5,1.5);

    general_condition_bar->SetSize(condition);
    general_condition_bar->Draw(x+0.5+0.005,y+0.26+0.02,1,1);
    //
    
}

void AidItem::Draw(GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    char temp[512];
    GLfloat w = 0.224;
    Item::Draw(x,y,xscale,yscale);
    box.SetSize(w,0.08,0.007);
    
    box.Draw(x+0.465+w*480/800,y+0.35);
    box.Draw(x+0.48+2*w*480/800,y+0.35);
    
    
    box.SetSize(w*3+(0.03*800/480),0.08,0.007);
    box.Draw(x+0.45,y+0.26);


    if(weight > 0) {
        snprintf(temp,sizeof(temp),"WG  %4.1f",weight);
        general_text->SetText( (const char*)&temp);
    }
    else
        general_text->SetText( "--");
    general_text->Draw(x+0.465+w*480/800+0.005,y+0.35+0.02,1.6,1.6);

    snprintf(temp,sizeof(temp),"VAL %4d",value);
    general_text->SetText( (const char*)&temp);
    general_text->Draw(x+0.48+2*w*480/800+0.005,y+0.35+0.02,1.6,1.6);
 

    snprintf(temp,sizeof(temp),"EFFECTS %25s",special);
    general_text->SetText( (const char*)&temp);
    general_text->Draw(x+0.45,y+0.26+0.02,1.4,1.4);

    //

}

void MiscItem::Draw(GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    char temp[512];
    GLfloat w = 0.224;
    Item::Draw(x,y,xscale,yscale);
    box.SetSize(w,0.08,0.007);
    
    box.Draw(x+0.465+w*480/800,y+0.35);
    box.Draw(x+0.48+2*w*480/800,y+0.35);
    
    

    if(weight > 0) {
        snprintf(temp,sizeof(temp),"WG  %4.1f",weight);
        general_text->SetText( (const char*)&temp);
    }
    else
        general_text->SetText( "--");
    general_text->Draw(x+0.465+w*480/800+0.005,y+0.35+0.02,1.6,1.6);

    snprintf(temp,sizeof(temp),"VAL %4d",value);
    general_text->SetText( (const char*)&temp);
    general_text->Draw(x+0.48+2*w*480/800+0.005,y+0.35+0.02,1.6,1.6);
 
}

void PanelItem::Draw(GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    char temp[512];
    GLfloat w = 0.224;
    Item::Draw(x,y,xscale,yscale);
    box.SetSize(w*2+(0.015*800/480),0.08,0.007);
    box.Draw(x+0.45,y+0.26);
    box.SetSize(w,0.08,0.007);
    box.Draw(x+0.48+2*w*480/800,y+0.26);
    box.SetSize(w*3+(0.03*800/480),0.08,0.007);
    box.Draw(x+0.45,y+0.35);

    general_text->SetText( name);
    general_text->Draw(x+0.45,y+0.35+0.02,1.6,1.6);

    general_text->SetText( (const char*)armour_class);
    general_text->Draw(x+0.45,y+0.26+0.02,1.6,1.6);

    general_text->SetText( (const char*)special);
    general_text->Draw(x+0.48+2*w*480/800+0.005,y+0.26+0.02,1.6,1.6);
 
}

void RadioItem::Draw(GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    //char temp[512];
    //GLfloat w = 0.224;
    //Item::Draw(x,y,xscale,yscale);
    /*box.SetSize(w*2+(0.015*800/480),0.08,0.007);
    box.Draw(x+0.45,y+0.26);
    box.SetSize(w,0.08,0.007);
    box.Draw(x+0.48+2*w*480/800,y+0.26);
    box.SetSize(w*3+(0.03*800/480),0.08,0.007);
    box.Draw(x+0.45,y+0.35);

    general_text->SetText( name );
    general_text->Draw(x+0.45,y+0.35+0.02,1.6,1.6);

    general_text->SetText( (const char*)armour_class );
    general_text->Draw(x+0.45,y+0.26+0.02,1.6,1.6);

    general_text->SetText( (const char*)special );
    general_text->Draw(x+0.48+2*w*480/800+0.005,y+0.26+0.02,1.6,1.6);*/
 
}


void AmmoItem::Draw(GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    char temp[512];
    GLfloat w = 0.224;
    Item::Draw(x,y,xscale,yscale);
    box.SetSize(w,0.08,0.007);
    
    box.Draw(x+0.465+w*480/800,y+0.35);
    box.Draw(x+0.48+2*w*480/800,y+0.35);
    
    

    if(weight > 0) {
        snprintf(temp,sizeof(temp),"WG  %4.1f",weight);
        general_text->SetText( (const char*)&temp );
    }
    else
        general_text->SetText( "--" );
    general_text->Draw(x+0.465+w*480/800+0.005,y+0.35+0.02,1.6,1.6);

    snprintf(temp,sizeof(temp),"VAL %4d",value);
    general_text->SetText( (const char*)&temp );
    general_text->Draw(x+0.48+2*w*480/800+0.005,y+0.35+0.02,1.6,1.6);

}

void ItemList::Draw(GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    //Draw the name list
    int count = 0;
    GLfloat equip_position = 0.79;
    for(itemlist_t::iterator i = itemlist.begin(); i != itemlist.end(); i++,count++) {
        int effective_count = count-display_start;
        if(count < display_start)
            continue;
        char *temp = strdup((*i)->text->text);
        if(temp) {
            if(strlen(temp) > 20) {
                temp[20] = '-';
                temp[21] = 0;
            }
            general_text->SetText( temp );
            general_text->Draw(x+0.17,y+0.81-(effective_count*0.08),1.4,1.4);
            free(temp);
        }
        if(i == current_item) {
            selected_box.Draw(x+0.135,y+0.786-(effective_count*0.08),1,1);
            if((*i)->type != RADIO && (*i)->type != EXIT) {
                selected_boxnub.Draw(x+0.14,y+0.808-(effective_count*0.08),1,1);
                if((*i)->code == equipped->weapon || (*i)->code == equipped->apparel_legs) {
                    general_text->SetText( "Unequip A)" );
                    general_text->Draw(equip_position,0.79,1.4,1.4);
                }
                else
                {
                    if((*i)->type == AID) {
                        general_text->SetText( "Use     A)" );
                        general_text->Draw(equip_position,0.79,1.4,1.4);
                    }
                    else if((*i)->type == PANEL) {}
                    else {
                        general_text->SetText( "Equip   A)" );
                        general_text->Draw(equip_position,0.79,1.4,1.4);
                    }
                }
            }
            else { //radio
                if(CurrentlyPlaying()) {
                    general_text->SetText( "Untune   A)" );
                    general_text->Draw(equip_position,0.79,1.4,1.4);
                }
                else {
                    general_text->SetText( "Tune     A)" );
                    general_text->Draw(equip_position,0.79,1.4,1.4);
                }
            }
            
        }
        if((*i)->code == equipped->weapon || (*i)->code == equipped->apparel_legs) {
            if((*i)->type != RADIO && (*i)->type != EXIT) {
                equipped_box->Draw(x+0.14,y+0.808-(effective_count*0.08),0.04,0.04);
            }
        }
        
        if(count >= display_start + display_num)
            break;
    }
    if(current_item != itemlist.end()) {
        (*current_item)->Draw(x,y,1,1);
    }
    if(itemlist.size() > display_num) {
        //draw the scroll bar
        //first the top arrow
        scrollbar->Draw(x,y,1,1);
    }
    return;
}

void ItemList::Next() {
    current_item++;
    current_pos++;
    if(current_item == itemlist.end())
    {
        current_item--;
        current_pos--;
    }
    else
    {
        PlayClip(*menu_prevnext);
        if(current_pos > display_start+display_num)
        {
            display_start++;
            scrollbar->SetData(itemlist.size(),display_start,display_num);
        }
    }
}

void ItemList::Prev() {
    if(current_item != itemlist.begin()) {
        PlayClip(*menu_prevnext);
        current_item--;
        if(current_pos == display_start) {
            //they can't be zero or current_item would = itemlist.begin() //I hope!
            display_start--;
            scrollbar->SetData(itemlist.size(),display_start,display_num);
            assert(display_start > 0 && current_pos > 0);
        }
        current_pos--;
    }
}
void ItemList::VerticalPos(float pos,int sound) {
    int new_item = ((int)(pos*itemlist.size()))%itemlist.size(); 
    if(new_item != current_pos) {
        //ah shit I didn't think of this when I did them as a linked list. should really sort this out, but for now...
        //could also go forward or backward from the current position n steps
        int diff = new_item - current_pos;
        //pthread_mutex_lock(&items_mutex);
        if(diff > 0) {
            while(diff) {
                current_item++;
                diff--;
            }
        }
        else {
            while(diff) {
                current_item--;
                diff++;
            }
        }  
        if(sound)
            PlayClip(*menu_prevnext);
        current_pos = new_item;
        if(current_pos > display_start+display_num)
        {
            display_start = current_pos - display_num;
            scrollbar->SetData(itemlist.size(),display_start,display_num);
        }
        else if(current_pos < display_start) {
            display_start = current_pos;
            scrollbar->SetData(itemlist.size(),display_start,display_num);
        }
        //pthread_mutex_unlock(&items_mutex);
    }
}

void ItemList::Select() {
    if(current_item != itemlist.end()) {
        (*current_item)->Select(equipped);
        if((*current_item)->type == AID) {
            itemlist_t::iterator todelete = current_item;
            current_item++;
            if(current_item == itemlist.end() && itemlist.size() > 1)
            {
                current_item--;
                current_item--;
            }
            itemlist.erase(todelete);
        }
    }
}

void WeaponItem::Select(EquippedItems *equipped){
    if(equipped->weapon == code)
    {
        equipped->weapon = NO_ITEM;
        if(unequip_weapon_sound != NULL)
            PlayClip(*unequip_weapon_sound);
    }
    else
    {
        equipped->weapon = code;
        if(equip_weapon_sound)
            PlayClip(*equip_weapon_sound);
    }
}
void ApparelItem::Select(EquippedItems *equipped){
    if(equipped->apparel_legs == code)
    {
        equipped->apparel_legs = NO_ITEM;
        if(unequip_apparel_sound != NULL)
            PlayClip(*unequip_apparel_sound);
    }
    else
    {
        equipped->apparel_legs = code;
        if(equip_apparel_sound != NULL)
            PlayClip(*equip_apparel_sound);
    }
}
void AidItem::Select(EquippedItems *equipped){
    if(equipped != NULL)
        PlayClip(*equip_aid_sound);
}
void MiscItem::Select(EquippedItems *equipped){
    if(equip_misc_sound != NULL)
        PlayClip(*equip_misc_sound);
}
void RadioItem::Select(EquippedItems *equipped){
    if(CurrentlyPlaying()){
        if(clip == last_played) {
            //stop the clip
            StopClip();
            last_played = NULL;
            return;
        }
    }
    if(clip != NULL) {
        PlayClip(*clip,0,1);
        last_played = clip;
    }
}

void ExitItem::Select(EquippedItems *equipped) {
    StopClip();
    throw ErrorMessage(USER_QUIT,"User requested exit");
}
