#include "SpotModifier.hpp"
#include "Engine.hpp"
#include <algorithm>
#include <cmath>
#include "JsonHelper.hpp"

namespace MsqLights {
    // https://stackoverflow.com/a/6853926
    float pDistance(Vector2 p, Vector2 a, Vector2 b) {

        float A = p.x - a.x;
        float B = p.y - a.y;
        float C = b.x - a.x;
        float D = b.y - a.y;

        float dot = A * C + B * D;
        float len_sq = C * C + D * D;
        float param = -1;
        if (len_sq != 0) //in case of 0 length line
            param = dot / len_sq;

        float xx, yy;

        if (param < 0) {
            xx = a.x;
            yy = a.y;
        }
        else if (param > 1) {
            xx = b.x;
            yy = b.y;
        }
        else {
            xx = a.x + param * C;
            yy = a.y + param * D;
        }

        float dx = p.x - xx;
        float dy = p.y - yy;
        return sqrt(dx * dx + dy * dy);
    }

    SpotModifier::SpotModifier(Engine* e)
    : Modifier(e) {
        type_ = "Spot";
        innerRadius_ = 0;
        outerRadius_ = 0;
    }

    float SpotModifier::AmountWithLine(Vector2 a, Vector2 b) {
        float dist = pDistance(position_, a, b);
        if (dist < innerRadius_)
            return 1.f;
        if (dist > outerRadius_)
            return 0.f;
        return 1.f - ((dist - innerRadius_) / (outerRadius_ - innerRadius_));
    }

    void SpotModifier::RegisterParameters() {
        Modifier::RegisterParameters();
        parameters_.emplace("pos.x", &position_.x);
        parameters_.emplace("pos.y", &position_.y);
        parameters_.emplace("innerRadius", &innerRadius_);
        parameters_.emplace("outerRadius", &outerRadius_);
    }
}