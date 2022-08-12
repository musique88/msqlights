#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include "Interfaces.hpp"
#include "Color.hpp"
#include "Modifiable.hpp"
#include "Vector2.hpp"

namespace MsqLights {
    class Modifier;

    class Fixture : public Modifiable {
    public: 
        enum class Mode {
            Dimmer, 
            RGB,
            RGBI,
            RGBA,
            RGBAW,
        };
        Vector2 position_;
        Vector2 lookingAt_;
        ColorInt color_;
        ColorInt nextColor_;
        ColorInt oldColor_;
        float fadeTimer_;
        float timeToFade_;
        unsigned int addr_;
        Mode mode_;
        std::string name_;

        static unsigned char cast(int a);

        ColorInt GetColor();
        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;
        Fixture(Engine* e);
        Fixture(const Fixture& f);
        Fixture(Engine* e, rapidjson::Value& val);
        Fixture(Engine* e, Vector2 position, Vector2 lookingAt, std::string name, unsigned int addr, Mode mode);
        ~Fixture();

        void RegisterParameters() override;

        void Blend(Modifier* m);
        void Update() override;
        void BlendTo(float secs);
        void WriteDmx();
    };
}
