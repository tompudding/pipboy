#include "gl_code.h"
#include <unistd.h>

ViewList::ViewList() {
    pthread_mutex_init(&viewlist_mutex,NULL);
    pthread_mutex_lock(&viewlist_mutex);
    viewlist.push_back(new StatsView("/mnt/sdcard/pygame/pipboy/stats_bg.png.raw",font));
    viewlist.push_back(new ItemsView("/mnt/sdcard/pygame/pipboy/items_bg.png.raw",font));
    viewlist.push_back(new DataView("/mnt/sdcard/pygame/pipboy/data_bg.png.raw",font));
    current_view = viewlist.begin();
    pthread_mutex_unlock(&viewlist_mutex);
}
ViewList::~ViewList() {
    for(viewlist_t::iterator i=viewlist.begin();i!=viewlist.end();i++) {
        if(*i != NULL)
            delete *i;
    }
}

View *ViewList::CurrentView()
{
    View *out = NULL;
    pthread_mutex_lock(&viewlist_mutex);
    if(current_view)
        out = *current_view;
    pthread_mutex_unlock(&viewlist_mutex);
    return out;
}
 
void ViewList::Next() {
    PlayClip(*mode_change);
    PlayClip(*mode_change_buzz,1);
    pthread_mutex_lock(&viewlist_mutex);
    current_view++;
    if(current_view == viewlist.end())
        current_view = viewlist.begin();
    pthread_mutex_unlock(&viewlist_mutex);
}

void ViewList::Prev()
{
    PlayClip(*mode_change);
    PlayClip(*mode_change_buzz,1);
    pthread_mutex_lock(&viewlist_mutex);
    if(current_view == viewlist.begin())
        current_view = viewlist.end();
    current_view--;
    pthread_mutex_unlock(&viewlist_mutex);
}

//Probably should just have this as one function...
void ViewList::UpInView()
{
    pthread_mutex_lock(&viewlist_mutex);
    (*current_view)->Up();
    pthread_mutex_unlock(&viewlist_mutex);
}

void ViewList::DownInView()
{
    pthread_mutex_lock(&viewlist_mutex);
    (*current_view)->Down();
    pthread_mutex_unlock(&viewlist_mutex);
}

void ViewList::LeftInView()
{
    pthread_mutex_lock(&viewlist_mutex);
    (*current_view)->Left();
    pthread_mutex_unlock(&viewlist_mutex);
}

void ViewList::RightInView()
{
    pthread_mutex_lock(&viewlist_mutex);
    (*current_view)->Right();
    pthread_mutex_unlock(&viewlist_mutex);
}

void ViewList::Select()
{
    LOGI("m2");
    pthread_mutex_lock(&viewlist_mutex);
    (*current_view)->Select();
    pthread_mutex_unlock(&viewlist_mutex);
}

void ViewList::SwitchView(uint32_t view, uint32_t rotary, float pos)
{
    uint32_t count = 0;
    viewlist_t::iterator i;
    uint32_t current_index = 0;

    for(i = viewlist.begin();i != viewlist.end(); i++,count++) {
        if (count == view) {
            break;
        }
    }
    if(count >= 3 || i == current_view)
        return;
 
    PlayClip(*mode_change);
    PlayClip(*mode_change_buzz,1);
    pthread_mutex_lock(&viewlist_mutex);
    current_view = i;
    (*current_view)->SetRotary(rotary,pos,0);
    pthread_mutex_unlock(&viewlist_mutex);
}

