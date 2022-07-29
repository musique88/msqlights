#pragma once
#include <raylib.h>

namespace MsqLights {
    unsigned char getChar(int a); 
    struct ColorInt {
        int r;
        int g;
        int b;
        int a;

        ColorInt();
        ColorInt(Color c);
        ColorInt(int r, int g, int b, int a);

        Color getColor();

        ColorInt operator+ (ColorInt lhs);
        ColorInt operator- (ColorInt lhs);
        ColorInt operator+ (Color lhs);
        ColorInt operator- (Color lhs);
        ColorInt operator* (float k);
        void operator= (Color lhs);
    };

    Color ColorMult(Color c, float k);

    Color ColorFade(Color a, Color b, float c);
}