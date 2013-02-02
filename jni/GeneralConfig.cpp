#include "gl_code.h"

GeneralConfig::GeneralConfig(const char *filename) {
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
    caps       = 61;
}
