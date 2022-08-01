#pragma once

#include "Modifier.hpp"

namespace MsqLights {
    class RectangleModifier : public Modifier {
    public:
        Rectangle rect_;
    public:
        RectangleModifier(Engine* e);

        void DrawProps() override;
        void Draw() override;
        Rectangle GetSelector() override;
        float AmountWithLine(Vector2 a, Vector2 b) override;
        void SetPosition(Vector2 p) override;  

        virtual void RegisterParameters() override;
    };
}