#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <raygui.h>
#include "Interfaces.hpp"
#include "Color.hpp"

namespace MsqLights {
    class Modifier;

    class Fixture : public Modifiable, public Serializable, public Drawable {
    public: 
        enum class Mode {
            Dimmer, 
            RGB,
            RGBA,
            RGBAW,
        };
        Vector2 position_;
        Vector2 lookingAt_;
        ColorInt color_;
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

        void Blend(Modifier* m);
        void Draw() override;
        void DrawProps() override;
        void WriteDmx();
    };
}
