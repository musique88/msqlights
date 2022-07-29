#pragma once

#include "Modifier.hpp"

namespace MsqLights {
    class RectangleModifier : public Modifier {
    public:
        Rectangle rect_;
    public:
        RectangleModifier(Engine* e);
        RectangleModifier(Engine* e, rapidjson::Value& val);

        virtual void SetParam(std::string str, float val) override;
        void DrawProps() override;
        void Draw() override;
        Rectangle GetSelector() override;
        float AmountWithLine(Vector2 a, Vector2 b) override;
        void SetPosition(Vector2 p) override;  
        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;
    };
}