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
    }

    ColorInt::ColorInt(int r, int g, int b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    ColorInt ColorInt::Clamped() {
        return {R(), G(), B()};
    }

    ColorInt ColorInt::operator+ (ColorInt lhs) {
        return (ColorInt) {r + lhs.r, g + lhs.g, b + lhs.b};
    }

    ColorInt ColorInt::operator- (ColorInt lhs) {
        return (ColorInt) {r - lhs.r, g - lhs.g, b - lhs.b};
    }
 
    ColorInt ColorInt::operator* (float k) {
        int nextR = r * k;
        int nextG = g * k;
        int nextB = b * k;

        return {
            nextR, nextG, nextB
        };
    }

    unsigned char ColorInt::R() {
        return getChar(r);
    }

    unsigned char ColorInt::G() {
        return getChar(g);
    }

    unsigned char ColorInt::B() {
        return getChar(b);
    }

    ColorInt ColorInt::Fade(ColorInt b, float k) {
        return (*this * k + b * (1 - k));
    }
}