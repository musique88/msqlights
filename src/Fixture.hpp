#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <raygui.h>
#include "Interfaces.hpp"
#include "Color.hpp"
#include "Modifiable.hpp"

namespace MsqLights {
    class Modifier;

    class Fixture : public Modifiable, public Drawable {
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
        Color oldColor_;
        float fadeTimer_;
        float timeToFade_;
        unsigned int addr_;
        Mode mode_;
        std::string name_;

        static unsigned int ModeToDMXSize(Mode mode);
        static unsigned char cast(int a);

        Color GetColor();
        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;
        Fixture(Engine* e);
        Fixture(const Fixture& f);
        Fixture(Engine* e, rapidjson::Value& val);
        Fixture(Engine* e, Vector2 position, Vector2 lookingAt, std::string name, unsigned int addr, Mode mode);
        ~Fixture();

        void RegisterParameters() override;

        void Blend(Modifier* m);
        void Draw() override;
        void Update();
        void BlendTo(float secs);
        void DrawProps() override;
        void WriteDmx();
    };
}
