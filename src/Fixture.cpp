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
                color_ = color_ + ColorMult(
                    m->color_, 
                    m->AmountWithLine(position_, lookingAt_)
                );
                return;
            case (Modifier::Blend::Subtract):
                color_ = color_ - ColorMult(
                    m->color_, 
                    m->AmountWithLine(position_, lookingAt_)
                );
                return;
        }
    }    

    void Fixture::Draw() {
        DrawCircleV(position_, 10, GetColor());
        DrawLineV(position_, lookingAt_, GetColor());
        DrawText(name_.c_str(), position_.x, position_.y, 20, engine_->selectedModifiable == this ? GREEN : WHITE);
    }

    void Fixture::DrawProps() {
        engine_->DisplayValue(&name_, "name", (Vector2){WIDTH - PANELSIZE, 20});
        engine_->DisplayValue(&position_.x, "position.x", (Vector2){WIDTH - PANELSIZE, 60}, 0, 1500);
        engine_->DisplayValue(&position_.y, "position.y", (Vector2){WIDTH - PANELSIZE, 100}, 0, 1080);
        engine_->DisplayValue(&lookingAt_.x, "lookingAt.x", (Vector2){WIDTH - PANELSIZE, 140}, 0, 1500);
        engine_->DisplayValue(&lookingAt_.y, "lookingAt.y", (Vector2){WIDTH - PANELSIZE, 180}, 0, 1080);
        engine_->DisplayPositionSelector(&position_, "Position", (Vector2){WIDTH - PANELSIZE, 220});
        engine_->DisplayPositionSelector(&lookingAt_, "LookingAt", (Vector2){WIDTH - PANELSIZE + 100, 220});
        DrawRectangleRec((Rectangle) {WIDTH - PANELSIZE + 200, 220, 220, 20}, GetColor());

        GuiLabel((Rectangle) {WIDTH - PANELSIZE, 240, 200, 20}, "addr");
        int val = addr_;
        if (GuiValueBox((Rectangle){WIDTH - PANELSIZE + 200, 240, 220, 20}, nullptr, &val, 0, 511, engine_->activeProp == (void*)&addr_)) {engine_->activeProp = (void*)&addr_;};
        addr_ = val;

        int selectedMode = (int)mode_;
        if (GuiDropdownBox((Rectangle) {WIDTH - PANELSIZE, 260, 300, 20}, "Dimmer;RGB;RGBA;RGBAW", &selectedMode, engine_->activeProp == &mode_)) {
            mode_ = (Mode)selectedMode;
            if (engine_->activeProp == &mode_)
                engine_->activeProp = nullptr;
            else
                engine_->activeProp = &mode_;
        }

        DrawRectangleRec((Rectangle) {WIDTH - PANELSIZE + 50, HEIGHT - 100, 370 * ((float)color_.r / 255.f), 20}, RED);
        GuiLabel((Rectangle) {WIDTH - PANELSIZE, HEIGHT - 100, 50, 20}, TextFormat("%d", color_.r));
        DrawRectangleRec((Rectangle) {WIDTH - PANELSIZE + 50, HEIGHT - 80, 370 * ((float)color_.g / 255.f), 20}, GREEN);
        GuiLabel((Rectangle) {WIDTH - PANELSIZE, HEIGHT - 80, 50, 20}, TextFormat("%d", color_.g));
        DrawRectangleRec((Rectangle) {WIDTH - PANELSIZE + 50, HEIGHT - 60, 370 * ((float)color_.b / 255.f), 20}, BLUE);
        GuiLabel((Rectangle) {WIDTH - PANELSIZE, HEIGHT - 60, 50, 20}, TextFormat("%d", color_.b));
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

    Fixture::~Fixture() {

    }
}