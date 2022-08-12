#include "Vector2.hpp"

namespace MsqLights {
    Vector2::Vector2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    Vector2::Vector2()
    : Vector2(0, 0) {}

    Vector2 Vector2::operator+(Vector2 rhs) {
        return Vector2 {x + rhs.x, y + rhs.y};
    };

    Vector2 Vector2::operator-(Vector2 rhs) {
        return Vector2 {x - rhs.x, y - rhs.y};
    };

    Vector2 Vector2::operator*(float rhs) {
        return Vector2 {x * rhs, y * rhs};
    };
}