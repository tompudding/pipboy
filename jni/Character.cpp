#include "gl_code.h"

Character::Character() {
    heads      = new Image(DATA_DIR "head.png.raw",0.25*480/800,0.25,standard_tex_coords);
    faces      = new Image(DATA_DIR "face.png.raw",0.125*480/800,0.125,standard_tex_coords);
    left_legs  = new Image(DATA_DIR "left_leg.png.raw",0.25*480/800,0.25,standard_tex_coords);
    right_legs = new Image(DATA_DIR "right_leg.png.raw",0.25*480/800,0.25,standard_tex_coords);
    right_arms = new Image(DATA_DIR "right_arm.png.raw",0.25*480/800,0.125,standard_tex_coords);
    left_arms  = new Image(DATA_DIR "left_arm.png.raw",0.25*480/800,0.125,standard_tex_coords);
    torsos     = new Image(DATA_DIR "torso.png.raw",0.25*480/800,0.25,standard_tex_coords);

    title      = new Text("Tom Gooding - Level 19",font);
}

void Character::Draw (GLfloat x,GLfloat y,GLfloat xscale, GLfloat yscale) {
    heads->Draw(     x + 0.45  , y + 0.5, xscale,yscale);
    faces->Draw(     x + 0.47  , y + 0.58 ,xscale,yscale);
    torsos->Draw(    x + 0.434 , y + 0.295,xscale,yscale);
    left_legs->Draw( x + 0.5   , y + 0.085,xscale,yscale);
    right_legs->Draw(x + 0.38  , y + 0.092,xscale,yscale);
    left_arms->Draw( x + 0.558 , y + 0.445,xscale,yscale);
    right_arms->Draw(x + 0.305 , y + 0.456,xscale,yscale);

    title->Draw(x + 0.37,y + 0.080,1.5,1.5);
}
