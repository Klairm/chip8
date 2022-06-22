#include "print.h"

Color bgColor = {0, 0, 0};


FT_Face fontSize;
const char *font = "/app0/assets/fonts/font.ttf";

void screenPrint(Scene2D *scene, char *text, int Ypos, int Xpos , Color textColor)
{
    scene->DrawText((char *)text, fontSize, Xpos, Ypos, bgColor, textColor);
    
}