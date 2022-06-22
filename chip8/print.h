#ifndef PRINT
#define PRINT

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <sstream>
#include "log.h"
#include "graphics.h"

#define FONT_SIZE 50
extern FT_Face fontSize;
extern const char *font;

void screenPrint(Scene2D *scene, char *text, int Ypos, int Xpos = 150, Color textColor = {255, 255, 255});

#endif