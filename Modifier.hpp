#pragma once

#include "Interfaces.hpp"
#include <vector>
#include <raylib.h>
#include "Fixture.hpp"
#include <string>

namespace MsqLights {
    class Engine;

    class Modifier : public Modifiable, public Drawable, public Serializable {
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
        std::string type_;

        Modifier(Engine* engine);
        Modifier(Engine* engine, rapidjson::Value& val);

        virtual void SetParam(std::string paramname, float val);

        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;
        void DrawProps() override;
        virtual float AmountWithLine(Vector2 a, Vector2 b) = 0; 
        virtual void Draw() override;
        virtual Rectangle GetSelector();
        virtual void SetPosition(Vector2 p) = 0;
        virtual void Update();
    };
}