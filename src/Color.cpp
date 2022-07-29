#include "Color.hpp"

namespace MsqLights {
    unsigned char getChar(int a) {
        if (a > 255)
            return 255;
        if (a < 0)
            return 0;

        return a;
    } 

    ColorInt::ColorInt() {
        r = 0;
        g = 0; 
        b = 0;
        a = 255;
    }

    ColorInt::ColorInt(Color c) {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
    }

    ColorInt::ColorInt(int r, int g, int b, int a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    Color ColorInt::getColor() {
        return {getChar(r), getChar(g), getChar(b), getChar(a)};
    }

    ColorInt ColorInt::operator+ (ColorInt lhs) {
        return (ColorInt) {r + lhs.r, g + lhs.g, b + lhs.b, a};
    }

    ColorInt ColorInt::operator- (ColorInt lhs) {
        return (ColorInt) {r - lhs.r, g - lhs.g, b - lhs.b, a};
    }

    ColorInt ColorInt::operator+ (Color lhs) {
        return (ColorInt) {r + lhs.r, g + lhs.g, b + lhs.b, a};
    }

    ColorInt ColorInt::operator- (Color lhs) {
        return (ColorInt) {r - lhs.r, g - lhs.g, b - lhs.b, a};
    }

    void ColorInt::operator= (Color lhs) {
        r = lhs.r;
        g = lhs.g;
        b = lhs.b;
        a = lhs.a;
    }
 
    ColorInt ColorInt::operator* (float k) {
        int nextR = r * k;
        int nextG = g * k;
        int nextB = b * k;

        return {
            nextR, nextG, nextB, a
        };
    }

    Color ColorMult(Color c, float k) {
        return (Color) {(unsigned char)((float)c.r * k), (unsigned char)((float)c.g * k) , (unsigned char)((float)c.b * k), c.a};
    }

    Color ColorFade(Color a, Color b, float k) {
        return (ColorInt(a) * k + ColorInt(b) * (1 - k)).getColor();
    }
}