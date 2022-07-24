#include "Color.hpp"

namespace MsqLights {
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
 
    Color ColorMult(Color c, float k) {
        return (Color) {(unsigned char)((float)c.r * k), (unsigned char)((float)c.g * k) , (unsigned char)((float)c.b * k), c.a};
    }
}