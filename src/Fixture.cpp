#include "Fixture.hpp"
#include "Engine.hpp"

#include "JsonHelper.hpp"

namespace MsqLights {
    unsigned char ResolveDimmer(Color c) {
        return std::max({c.r, c.g, c.b});
    }

    std::array<unsigned char, 3> ResolveRGB(Color c) {
        std::array<unsigned char, 3> arr;
        arr[0] = c.r;
        arr[1] = c.g;
        arr[2] = c.b;
        return arr;
    }

    std::array<unsigned char, 4> ResolveRGBI(Color c) {
        std::array<unsigned char, 4> arr;
        arr[0] = c.r;
        arr[1] = c.g;
        arr[2] = c.b;
        arr[3] = 255;
        return arr;
    }

    std::array<unsigned char, 4> ResolveRGBA(Color c) {
        std::array<unsigned char, 4> arr;
        auto rgb = ResolveRGB(c);
        arr[0] = rgb[0];
        arr[1] = rgb[1];
        arr[2] = rgb[2];

        float rg = c.r - c.g;
        if (rg < 0)
            rg = 0;

        int average = ((int)rgb[0] + (int)rgb[1]) / 2;
        int a = 0;
        a = average - rg;
        if (a < 0) a = 0;
        if (a > 255) a = 255;

        arr[3] = a;

        return arr;
    }

    std::array<unsigned char, 5> ResolveRGBAW(Color c) {
        std::array<unsigned char, 5> arr;
        auto rgba = ResolveRGBA(c);
        arr[0] = rgba[0];
        arr[1] = rgba[1];
        arr[2] = rgba[2];
        arr[3] = rgba[3];

        int rg = std::abs(c.r - c.g);
        int gb = std::abs(c.g - c.b);

        int average = ((int)rgba[0] + (int)rgba[1] + (int)rgba[2]) / 3;
        int w = 0;
        w = average - std::max({rg, gb});
        if (w < 0) w = 0;
        if (w > 255) w = 255;
        arr[4] = w;

        return arr;
    }

    unsigned int Fixture::ModeToDMXSize(Mode mode) {
        switch(mode) {
            case Mode::Dimmer: 
                return 1;
            case Mode::RGB:
                return 3;
            case Mode::RGBA:
                return 4;
            case Mode::RGBAW:
                return 5;
        };
    }

    unsigned char Fixture::cast(int a) {
        if (a > 255)
            return 255;
        if (a < 0)
            return 0;
        return a;
    }

    Color Fixture::GetColor() {
        return (Color) {cast(color_.r), cast(color_.g), cast(color_.b), cast(color_.a)};
    }

    rapidjson::Value Fixture::Serialize(rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value val = Modifiable::Serialize(allocator);
        rapidjson::Value name;
        name.SetString(name_.c_str(), allocator);
        val.AddMember("name", name, allocator);
        val.AddMember("position", MsqLights::Serialize(position_, allocator), allocator);
        val.AddMember("lookingAt", MsqLights::Serialize(lookingAt_, allocator), allocator);
        val.AddMember("addr", addr_, allocator);
        val.AddMember("mode", (int)mode_, allocator);
        return val;
    }

    Fixture::Fixture(Engine* e, rapidjson::Value& val)
    : Modifiable(e) {
        name_ = val["name"].GetString();
        position_ = Vector2Parse(val["position"]);
        lookingAt_ = Vector2Parse(val["lookingAt"]);
        addr_ = val["addr"].GetInt();
        mode_ = (Mode)val["mode"].GetInt();
    }

    Fixture::Fixture(Engine* e)
    : Modifiable(e) {
        position_ = (Vector2) {0, 0};
        lookingAt_ = (Vector2) {0, 0};
        color_ = (Color) {0,0,0,255};
        addr_ = 0;
        mode_ = Mode::Dimmer;
        name_ = "Fixture";
        fadeTimer_ = -0.1;
        timeToFade_ = 1;
    }

    Fixture::Fixture(const Fixture& f) 
    : Fixture(f.engine_){
        position_ = f.position_;
        lookingAt_ = f.lookingAt_;
        color_ = f.color_;
        addr_ = f.addr_;
        mode_ = f.mode_;
        name_ = std::string(f.name_);
    }

    Fixture::Fixture(Engine* e, Vector2 position, Vector2 lookingAt, std::string name, unsigned int addr, Mode mode) 
    : Fixture(e) {
        mode_ = mode;
        position_ = position;
        lookingAt_ = lookingAt;
        name_ = name;
        addr_ = addr;
        color_ = (Color){0,0,0,255};
    }

    void Fixture::Blend(Modifier* m) {
        switch (m->blendMode_){
            case (Modifier::Blend::Addition):
                nextColor_ = nextColor_ + ColorMult(
                    m->color_, 
                    m->AmountWithLine(position_, lookingAt_)
                );
                return;
            case (Modifier::Blend::Subtract):
                nextColor_ = nextColor_ - ColorMult(
                    m->color_, 
                    m->AmountWithLine(position_, lookingAt_)
                );
                return;
        }
    }    

    void Fixture::RegisterParameters() {
        parameters_.emplace("name", &name_);
        parameters_.emplace("position.x", &position_.x);
        parameters_.emplace("position.y", &position_.y);
        parameters_.emplace("lookingAt.x", &lookingAt_.x);
        parameters_.emplace("lookingAt.y", &lookingAt_.y);
        parameters_.emplace("addr", &addr_);
        parameters_.emplace("mode", (unsigned int*) &mode_);
    }

    void Fixture::WriteDmx() {
        auto arr = ResolveRGBAW(GetColor());

        switch (mode_)
        {
        case Mode::Dimmer:
            engine_->dmxValues[addr_] = ResolveDimmer(GetColor());
            break;

        case Mode::RGB:
            engine_->dmxValues[addr_] = arr[0];
            engine_->dmxValues[addr_ + 1] = arr[1];
            engine_->dmxValues[addr_ + 2] = arr[2];
            break;

        case Mode::RGBI:
            engine_->dmxValues[addr_] = arr[0];
            engine_->dmxValues[addr_ + 1] = arr[1];
            engine_->dmxValues[addr_ + 2] = arr[2];
            engine_->dmxValues[addr_ + 3] = 255;
            break;

        case Mode::RGBA:
            engine_->dmxValues[addr_] = arr[0];
            engine_->dmxValues[addr_ + 1] = arr[1];
            engine_->dmxValues[addr_ + 2] = arr[2];
            engine_->dmxValues[addr_ + 3] = arr[3];
            break;

        case Mode::RGBAW:
            engine_->dmxValues[addr_] = arr[0];
            engine_->dmxValues[addr_ + 1] = arr[1];
            engine_->dmxValues[addr_ + 2] = arr[2];
            engine_->dmxValues[addr_ + 3] = arr[3];
            engine_->dmxValues[addr_ + 4] = arr[4];
            break;
        }
    }

    void Fixture::Update() {
        color_ = nextColor_;
        nextColor_ = BLACK;
        if(fadeTimer_ <= 0)
            return;
        
        color_ = (
            ColorInt(oldColor_) * (fadeTimer_ / timeToFade_) + 
            (color_) * (1 - (fadeTimer_ / timeToFade_)));

        fadeTimer_ -= GetFrameTime();
    }

    void Fixture::BlendTo(float secs) {
        oldColor_ = color_.getColor();
        fadeTimer_ = secs;
        timeToFade_ = secs;
    }

    Fixture::~Fixture() {

    }
}