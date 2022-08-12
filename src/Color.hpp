#pragma once

namespace MsqLights {
    unsigned char getChar(int a); 
    struct ColorInt {
        int r;
        int g;
        int b;

        ColorInt();
        ColorInt(int r, int g, int b);

        ColorInt Clamped();
        ColorInt Fade(ColorInt b, float k);

        ColorInt operator+ (ColorInt lhs);
        ColorInt operator- (ColorInt lhs);
        ColorInt operator* (float k);
        unsigned char R();
        unsigned char G();
        unsigned char B();
    };
}