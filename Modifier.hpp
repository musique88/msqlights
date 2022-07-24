#pragma once

#include "Interfaces.hpp"
#include <vector>
#include <raylib.h>
#include "Fixture.hpp"
#include <string>

namespace MsqLights {
    class Engine;

    class Modifier : public Modifiable, public Drawable {
    protected:
        int blendModeGui_ = 0;
    public:
        enum class Blend {
            Addition,
            Subtract
        };
        Blend blendMode_;
        Color color_;
        std::vector<Fixture*> affectedFixtures_;
        std::string name_;

        Modifier(Engine* engine);

        void DrawProps() override;
        virtual float AmountWithLine(Vector2 a, Vector2 b) = 0; 
        virtual void Draw() override;
        virtual Rectangle GetSelector();
        virtual void SetPosition(Vector2 p) = 0;
    };
}