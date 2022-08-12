#pragma once

#include "Interfaces.hpp"
#include "Modifiable.hpp"
#include <vector>
#include <raylib.h>
#include "Fixture.hpp"
#include <string>
#include <map>

namespace MsqLights {
    class Engine;

    class Modifier : public Modifiable {
    protected:
        int blendModeGui_ = 0;
        ColorInt tempColor_ = {0,0,0};
        float fadeTimer_ = 0;
        ColorInt nextColor_ = {0,0,0};
        ColorInt oldColor_ = {0,0,0};
        bool fadeDone_ = true;
    public:
        enum class Blend {
            Addition,
            Subtract
        };
        Blend blendMode_;
        float fade_;
        ColorInt color_;
        std::vector<unsigned int> affectedFixtures_;
        std::string name_;
        std::string type_;

        Modifier(Engine* engine);
        ~Modifier();

        virtual void SetParameter(std::string paramname, float val);

        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;
        virtual float AmountWithLine(Vector2 a, Vector2 b) = 0; 
        virtual void Update() override;
        virtual void RegisterParameters() override;
        virtual void Load(rapidjson::Value& val) override;
    };
}