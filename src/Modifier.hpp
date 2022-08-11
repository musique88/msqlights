#pragma once

#include "Interfaces.hpp"
#include "Modifiable.hpp"
#include <vector>
#include <raylib.h>
#include "Fixture.hpp"
#include <string>
#include "Parameter.hpp"
#include <map>

namespace MsqLights {
    class Engine;

    class Modifier : public Modifiable, public Drawable {
    protected:
        int blendModeGui_ = 0;
        Color tempColor_ = {0,0,0,255};
        float fadeTimer_ = 0;
        Color nextColor_ = {0,0,0,255};
        Color oldColor_ = {0,0,0,255};
        bool fadeDone_ = true;
    public:
        enum class Blend {
            Addition,
            Subtract
        };
        Blend blendMode_;
        float fade_;
        Color color_;
        std::vector<Fixture*> affectedFixtures_;
        std::string name_;
        std::string type_;

        Modifier(Engine* engine);
        ~Modifier();

        virtual void SetParameter(std::string paramname, float val);

        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;
        void DrawProps() override;
        virtual float AmountWithLine(Vector2 a, Vector2 b) = 0; 
        virtual void Draw() override;
        virtual Rectangle GetSelector();
        virtual void Update();
        virtual void RegisterParameters() override;
        virtual void Load(rapidjson::Value& val) override;
    };
}