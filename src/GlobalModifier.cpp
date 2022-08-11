#include "GlobalModifier.hpp"
#include "Engine.hpp"
#include "JsonHelper.hpp"

namespace MsqLights {
    GlobalModifier::GlobalModifier(Engine* e)
    : Modifier(e) {
        type_ = "Global";
    }

    float GlobalModifier::AmountWithLine(Vector2 a, Vector2 b) {
        return 1;
    }

    void GlobalModifier::RegisterParameters() {
        Modifier::RegisterParameters();
    }
}