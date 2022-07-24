#pragma once
#include "Modifier.hpp"

namespace MsqLights {
    class SpotModifier : public Modifier {
    public:
        Vector2 position_;
        float innerRadius_;
        float outerRadius_;
    public:
        SpotModifier(Engine* e);

        virtual void DrawProps() override;
        virtual void Draw() override;
        Rectangle GetSelector() override;
        float AmountWithLine(Vector2 a, Vector2 b) override;
        void SetPosition(Vector2 p) override;  
    };
}