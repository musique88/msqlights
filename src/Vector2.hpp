#pragma once

namespace MsqLights {
    class Vector2 {
    public:
        float x;
        float y;
        Vector2(float x, float y);
        Vector2();
        Vector2 operator+(Vector2 rhs);
        Vector2 operator-(Vector2 rhs);
        Vector2 operator*(float rhs);
    };
}