#pragma once
#include <raylib.h>

namespace MsqLights {
    struct ColorInt {
        int r;
        int g;
        int b;
        int a;

        ColorInt operator+ (ColorInt lhs);
        ColorInt operator- (ColorInt lhs);
        ColorInt operator+ (Color lhs);
        ColorInt operator- (Color lhs);
        void operator= (Color lhs);
    };

    Color ColorMult(Color c, float k);
}