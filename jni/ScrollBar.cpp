#include "gl_code.h"
#include <unistd.h>

ScrollBar::ScrollBar() {
    top = new Image(DATA_DIR "chevrons.dimensions_64x32.raw",480.0/800,1,standard_tex_coords);
    bottom = new Image(DATA_DIR "chevrons1.dimensions_64x32.raw",480.0/800,1,standard_tex_coords);
    bar = new Image(DATA_DIR "bar.dimensions_64x64.raw",480.0/800,1,standard_tex_coords);
}

ScrollBar::~ScrollBar() {
    if(NULL != top)
        delete top;
    if(NULL != bottom)
        delete bottom;
}

void ScrollBar::SetData(uint32_t a,uint32_t b, uint32_t c) {
    total_items = a;
    current_item = b;
    window_size = c;
}

void ScrollBar::Draw(GLfloat x, GLfloat y,GLfloat xscale, GLfloat yscale) {
    if(NULL != top)
        top->Draw(x+0.093,y+0.825,0.1,0.05);
    if(NULL != bottom)
        bottom->Draw(x+0.093,y+0.14,0.1,0.05);
    if(NULL != bar) {
        GLfloat barx = x+0.119;
        int32_t bottom_space = total_items-(current_item+window_size);
        if(bottom_space < 0)
            bottom_space = 0;
        GLfloat bary = y+(0.14 + (((float)bottom_space)/total_items)*(0.825-0.14));
        GLfloat barsize = (((float)window_size)/total_items)*(0.825-0.14);
        //if(barsize > 0.3)
        //{
        //    GLfloat adjust = barsize-0.3;
        //    barsize = 0.3;
        //    bary += adjust/2;
        //}
    
        bar->Draw(barx,bary,0.007,barsize);
    }
}
