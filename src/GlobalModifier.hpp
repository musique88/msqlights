#pragma once

#include "Modifier.hpp"

namespace MsqLights {
    class GlobalModifier : public Modifier {
    public:
        GlobalModifier(Engine* e);

        void DrawProps() override;
        void Draw() override;
        Rectangle GetSelector() override;
        float AmountWithLine(Vector2 a, Vector2 b) override;

        virtual void RegisterParameters() override;
    };
}