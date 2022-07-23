#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include "raygui.h"
#include "dark.h"

#include <string>
#include <algorithm>
#include <vector>
#include <cstring>
#include <iostream>
#include <cmath>
#include <array>

#include <ola/DmxBuffer.h>
#include <ola/client/StreamingClient.h>

void* activeProp = nullptr;
const unsigned int dmxAddresses = 512;
ola::client::StreamingClient dmxClient;

unsigned char dmxValues[dmxAddresses];

// https://stackoverflow.com/a/6853926
float pDistance(Vector2 p, Vector2 a, Vector2 b) {
    
    float A = p.x - a.x;
    float B = p.y - a.y;
    float C = b.x - a.x;
    float D = b.y - a.y;

    float dot = A * C + B * D;
    float len_sq = C * C + D * D;
    float param = -1;
    if (len_sq != 0) //in case of 0 length line
        param = dot / len_sq;

    float xx, yy;

    if (param < 0) {
        xx = a.x;
        yy = a.y;
    }
    else if (param > 1) {
        xx = b.x;
        yy = b.y;
    }
    else {
        xx = a.x + param * C;
        yy = a.y + param * D;
    }

    float dx = p.x - xx;
    float dy = p.y - yy;
    return sqrt(dx * dx + dy * dy);
}

struct ColorInt {
    int r;
    int g;
    int b;
    int a;

    ColorInt operator+ (ColorInt lhs) {
        return (ColorInt) {r + lhs.r, g + lhs.g, b + lhs.b, a};
    }

    ColorInt operator- (ColorInt lhs) {
        return (ColorInt) {r - lhs.r, g - lhs.g, b - lhs.b, a};
    }

    ColorInt operator+ (Color lhs) {
        return (ColorInt) {r + lhs.r, g + lhs.g, b + lhs.b, a};
    }

    ColorInt operator- (Color lhs) {
        return (ColorInt) {r - lhs.r, g - lhs.g, b - lhs.b, a};
    }

    void operator= (Color lhs) {
        r = lhs.r;
        g = lhs.g;
        b = lhs.b;
        a = lhs.a;
    }
};

Color ColorMult(Color c, float k) {
    return (Color) {(unsigned char)((float)c.r * k), (unsigned char)((float)c.g * k) , (unsigned char)((float)c.b * k), c.a};
}

unsigned char SafeAdd(unsigned char a, unsigned char b) {
    int ans = (int)a + (int)b;
    if(ans > 255)
        return 255;
    return ans;
}

Color SafeAdd(Color a, Color b) {
    return (Color) {SafeAdd(a.r, b.r), SafeAdd(a.g, b.g), SafeAdd(a.b, b.b), 255};
}

unsigned char SafeSub(unsigned char a, unsigned char b) {
    int ans = (int)a - (int)b;
    if(ans < 0)
        return 0;
    return ans;
}

Color SafeSub(Color a, Color b) {
    return (Color) {SafeSub(a.r, b.r), SafeSub(a.g, b.g), SafeSub(a.b, b.b), 255};
}

class Serializable {
public:
    virtual std::vector<char> Serialize() = 0;
};

class Modifiable {
public:
    virtual void DrawProps() = 0;
};

class Fixture;
class Drawable {
public:
    virtual void Draw() = 0;
};

std::vector<Fixture*> fixtures;
Modifiable* selectedModifiable;
Vector2* positionSelector = nullptr;

void DisplayValue(int* value, std::string propName, Vector2 pos, int min, int max) {
    GuiLabel((Rectangle){pos.x, pos.y, 300, 20}, propName.c_str());
    *value = GuiSliderBar((Rectangle){pos.x, pos.y + 20, 420, 20}, nullptr, nullptr, *value, min, max);
    if (GuiValueBox((Rectangle){pos.x + 300, pos.y, 120, 20}, nullptr, value, min, max, activeProp == (void*)value)) {activeProp = (void*)value;};
}

void DisplayValue(float* value, std::string propName, Vector2 pos, int min, int max) {
    GuiLabel((Rectangle){pos.x, pos.y, 300, 20}, propName.c_str());
    *value = GuiSliderBar((Rectangle){pos.x, pos.y + 20, 420, 20}, nullptr, nullptr, *value, min, max);
    int val = *value;
    if (GuiValueBox((Rectangle){pos.x + 300, pos.y, 120, 20}, nullptr, &val, min, max, activeProp == (void*)value)) {activeProp = (void*)value;};
    *value = val;
}

void DisplayValue(std::string* value, std::string propName, Vector2 pos) {
    GuiLabel((Rectangle){pos.x, pos.y, 420, 20}, propName.c_str());
    char name[value->size() + 1];
    std::copy(value->begin(), value->end(), name);
    name[value->size()] = '\0';
    if (GuiTextBox((Rectangle){pos.x, pos.y + 20, 420, 20}, name, 16, activeProp == (void*)value)) {activeProp = (void*)value;};
    *value = name;
}

void DisplayPositionSelector(Vector2* position, std::string propName, Vector2 pos) {
    if(GuiButton((Rectangle) {pos.x, pos.y, 100, 20}, propName.c_str())) {
        positionSelector = position;
    }
}

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

    float closenessToR = c.r == 255 ? 1 : 1 / (c.r - 255);
    float closenessToG = c.g == 50 ? 1 : 1 / (c.g - 50);
    float closeNessToA = std::abs(closenessToR * closenessToG);
    arr[3] = closeNessToA * 255;

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
    int threshold = 100 * ((float)average / 255);
    int w = 0;
    w = average - std::max({rg, gb});
    if (w < 0) w = 0;
    if (w > 255) w = 255;
    arr[4] = w;

    return arr;
}

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

    static unsigned int ModeToDMXSize(Mode mode) {
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

    static unsigned char cast(int a) {
        if (a > 255)
            return 255;
        if (a < 0)
            return 0;
        return a;
    }

    Color GetColor() {
        return (Color) {cast(color_.r), cast(color_.g), cast(color_.b), cast(color_.a)};
    }

    std::vector<char> Serialize() override {
        std::vector<char> v;
        for(unsigned int i = 0; i < sizeof(position_); i++)
            v.push_back(((char*)&position_)[i]);
        for(unsigned int i = 0; i < sizeof(lookingAt_); i++)
            v.push_back(((char*)&lookingAt_)[i]);
        for(unsigned int i = 0; i < sizeof(color_); i++)
            v.push_back(((char*)&color_)[i]);
        for(unsigned int i = 0; i < sizeof(addr_); i++)
            v.push_back(((char*)&addr_)[i]);
        for(unsigned int i = 0; i < sizeof(mode_); i++)
            v.push_back(((char*)&mode_)[i]);
        const char* name = name_.c_str();
        unsigned int namesize = name_.size();
        for(unsigned int j = 0; j <= namesize; j++)
            v.push_back(name[j]);
        return v;
    }

    Fixture() {

    }

    Fixture(const Fixture& f) {
        position_ = f.position_;
        lookingAt_ = f.lookingAt_;
        color_ = f.color_;
        addr_ = f.addr_;
        mode_ = f.mode_;
        name_ = std::string(f.name_);
    }

    Fixture(unsigned char* data) {
        unsigned char* ptr = data;
        position_ = *(Vector2*) ptr;
        ptr += sizeof(Vector2);
        lookingAt_ = *(Vector2*) ptr;
        ptr += sizeof(Vector2);
        color_ = *(ColorInt*) ptr;
        ptr += sizeof(ColorInt);

        addr_ = *(unsigned int*) ptr;
        ptr += sizeof(unsigned int);
        mode_ = *(Mode*) ptr;
        ptr += sizeof(Mode);

        name_ = std::string((char*)ptr);
    }

    Fixture(Vector2 position, Vector2 lookingAt, std::string name, unsigned int addr, Mode mode) 
    : mode_(mode), position_(position), lookingAt_(lookingAt), name_(name), addr_(addr){
        color_ = (Color){0,0,0,255};
    }

    void Draw() override {
        DrawCircleV(position_, 10, GetColor());
        DrawLineV(position_, lookingAt_, GetColor());
        DrawText(name_.c_str(), position_.x, position_.y, 20, selectedModifiable == this ? GREEN : WHITE);
    }

    void DrawProps() override {
        DisplayValue(&name_, "name", (Vector2){1500, 20});
        DisplayValue(&position_.x, "position.x", (Vector2){1500, 60}, 0, 1500);
        DisplayValue(&position_.y, "position.y", (Vector2){1500, 100}, 0, 1080);
        DisplayValue(&lookingAt_.x, "lookingAt.x", (Vector2){1500, 140}, 0, 1500);
        DisplayValue(&lookingAt_.y, "lookingAt.y", (Vector2){1500, 180}, 0, 1080);
        DisplayPositionSelector(&position_, "Position", (Vector2){1500, 220});
        DisplayPositionSelector(&lookingAt_, "LookingAt", (Vector2){1600, 220});
        DrawRectangleRec((Rectangle) {1700, 220, 220, 20}, GetColor());

        GuiLabel((Rectangle) {1500, 240, 200, 20}, "addr");
        int val = addr_;
        if (GuiValueBox((Rectangle){1700, 240, 220, 20}, nullptr, &val, 0, 512, activeProp == (void*)&addr_)) {activeProp = (void*)&addr_;};
        addr_ = val;

        int selectedMode = (int)mode_;
        if (GuiDropdownBox((Rectangle) {1500, 260, 300, 20}, "Dimmer;RGB;RGBA;RGBAW", &selectedMode, activeProp == &mode_)) {
            mode_ = (Mode)selectedMode;
            if (activeProp == &mode_)
                activeProp = nullptr;
            else
                activeProp = &mode_;
        }

        DrawRectangleRec((Rectangle) {1550, 980, 370 * ((float)color_.r / 255.f), 20}, RED);
        GuiLabel((Rectangle) {1500, 980, 50, 20}, TextFormat("%d", color_.r));
        DrawRectangleRec((Rectangle) {1550, 1000, 370 * ((float)color_.g / 255.f), 20}, GREEN);
        GuiLabel((Rectangle) {1500, 1000, 50, 20}, TextFormat("%d", color_.g));
        DrawRectangleRec((Rectangle) {1550, 1020, 370 * ((float)color_.b / 255.f), 20}, BLUE);
        GuiLabel((Rectangle) {1500, 1020, 50, 20}, TextFormat("%d", color_.b));
    }

    void WriteDmx() {
        auto arr = ResolveRGBAW(GetColor());

        switch (mode_)
        {
        case Mode::Dimmer:
            dmxValues[addr_] = ResolveDimmer(GetColor());
            break;

        case Mode::RGB:
            dmxValues[addr_] = arr[0];
            dmxValues[addr_ + 1] = arr[1];
            dmxValues[addr_ + 2] = arr[2];
            break;

        case Mode::RGBA:
            dmxValues[addr_] = arr[0];
            dmxValues[addr_ + 1] = arr[1];
            dmxValues[addr_ + 2] = arr[2];
            dmxValues[addr_ + 3] = arr[3];
            break;

        case Mode::RGBAW:
            dmxValues[addr_] = arr[0];
            dmxValues[addr_ + 1] = arr[1];
            dmxValues[addr_ + 2] = arr[2];
            dmxValues[addr_ + 3] = arr[3];
            dmxValues[addr_ + 4] = arr[4];
            break;

        default:
            break;
        }
    }
};

class Modifier : public Modifiable, public Drawable {
protected:
    int blendModeGui_ = 0;
public:
    enum class Blend {
        Addition,
        Subtract
    } blendMode_ = Blend::Addition;
    Color color_ = {0, 0, 0, 255};
    std::vector<Fixture*> affectedFixtures_;
    std::string name_;

    void DrawProps() override {
        std::string s("Addition;Subtract");

        DisplayValue(&name_, "name", (Vector2){1500, 20});
        if (GuiDropdownBox((Rectangle) {1500, 60, 420, 20}, "Addition;Subtract", &blendModeGui_, activeProp == &blendMode_)){
            blendMode_ = (Blend)blendModeGui_;
            if (activeProp == &blendMode_)
                activeProp = nullptr;
            else
                activeProp = &blendMode_;
        }

        color_ = GuiColorPicker((Rectangle){1520, 500, 370, 100}, "Color", color_);
        DrawRectangleRec((Rectangle) {1500, 500, 20, 100}, color_);
        for(unsigned int i = 0; i < fixtures.size(); i++) {
            auto position = std::find(affectedFixtures_.begin(), affectedFixtures_.end(), fixtures[i]);
            bool actived = position != affectedFixtures_.end();
            bool nextActived = GuiToggle((Rectangle) {1500, 600 + (float)i * 20, 420, 20}, fixtures[i]->name_.c_str(), actived);
            bool clicked = actived != nextActived;
            if(clicked) {
                if(!actived)
                    affectedFixtures_.erase(position);
                else
                    affectedFixtures_.push_back(fixtures[i]);
            }
        }
        GuiLabel((Rectangle) {1500, 600, 100, 20}, TextFormat("%d", affectedFixtures_.size()));
    }

    virtual float AmountWithLine(Vector2 a, Vector2 b) = 0; 

    virtual void Draw() override {};

    virtual Rectangle GetSelector() {};
};

std::vector<Modifier*> modifiers;
class RectangleModifier : public Modifier {
public:
    Rectangle rect_;
    void DrawProps() override {
        DisplayValue(&rect_.x, "rect.x", (Vector2) {1500, 80}, 0, 1500);
        DisplayValue(&rect_.y, "rect.y", (Vector2) {1500, 120}, 0, 1080);
        DisplayValue(&rect_.width, "rect.w", (Vector2) {1500, 160}, 0, 1500);
        DisplayValue(&rect_.height, "rect.h", (Vector2) {1500, 200}, 0, 1080);
        DisplayPositionSelector((Vector2*)&rect_, "Origin", (Vector2) {1500, 240});
        Modifier::DrawProps();
    }

    void Draw() override {
        DrawRectangleRec(GetSelector(), color_);
        DrawRectangleLinesEx(rect_, 3, color_);
        DrawText(name_.c_str(), rect_.x, rect_.y, 20, selectedModifiable == this ? GREEN : BLUE);
    }
    
    Rectangle GetSelector() override {
        return (Rectangle) {rect_.x - 10, rect_.y - 10, 20, 20};
    }

    float AmountWithLine(Vector2 a, Vector2 b) override {
        if (CheckCollisionPointRec(a, rect_)) return 1.f;
        if (CheckCollisionPointRec(b, rect_)) return 1.f;
        bool collision = false;
        Vector2 p;
        Vector2 topLeft = *(Vector2*)&rect_;
        Vector2 topRight = topLeft;
        topRight.x += rect_.width;
        Vector2 botLeft = topLeft;
        botLeft.y += rect_.height;
        Vector2 botRight = botLeft;
        botRight.x += rect_.width;
        collision |= CheckCollisionLines(topLeft, topRight, a, b, &p);
        collision |= CheckCollisionLines(topRight, botRight, a, b, &p);
        collision |= CheckCollisionLines(botRight, botLeft, a, b, &p);
        collision |= CheckCollisionLines(botLeft, topLeft, a, b, &p);
        if (collision) return 1.f;
        return 0.f;
    }
};

class SpotModifier : public Modifier {
public:
    Vector2 position_;
    float innerRadius_;
    float outerRadius_;
    virtual void DrawProps() override {
        DisplayValue(&position_.x, "pos.x", (Vector2) {1500, 80}, 0, 1500);
        DisplayValue(&position_.y, "pos.y", (Vector2) {1500, 120}, 0, 1080);
        DisplayValue(&innerRadius_, "innerRadius", (Vector2) {1500, 160}, 0, 1080);
        DisplayValue(&outerRadius_, "outerRadius", (Vector2) {1500, 200}, 0, 1080);
        DisplayPositionSelector(&position_, "Origin", (Vector2) {1500, 240});
        Modifier::DrawProps();
    }

    virtual void Draw() override {
        DrawRectangleRec(GetSelector(), color_);
        DrawCircleLines(position_.x, position_.y, innerRadius_, color_);
        DrawCircleLines(position_.x, position_.y, outerRadius_, color_);
        DrawText(name_.c_str(), position_.x, position_.y, 20, selectedModifiable == this ? GREEN : BLUE);
    }
    
    Rectangle GetSelector() override {
        return (Rectangle) {position_.x - 10, position_.y - 10, 20, 20};
    }

    float AmountWithLine(Vector2 a, Vector2 b) override {
        float dist = pDistance(position_, a, b);
        if (dist < innerRadius_)
            return 1.f;
        if (dist > outerRadius_)
            return 0.f;
        return 1.f - ((dist - innerRadius_) / (outerRadius_ - innerRadius_));
    }
};

class Follow : public SpotModifier {
public:
    bool enabled_;
    bool followingMouse_;
    Follow() {
        name_ = "FOLLOW";
        enabled_ = false;
    }
    void DrawProps() override {
        GuiLabel((Rectangle) {1500, 300, 420, 20}, "Follow (Press f to toggle)");
        DrawRectangleRec((Rectangle) {1700, 300, 10, 10}, enabled_ ? GREEN : RED);
        color_ = GuiColorPicker((Rectangle){1520, 500, 370, 100}, "Color", color_);
        DrawRectangleRec((Rectangle) {1500, 500, 20, 100}, color_);
        for(unsigned int i = 0; i < fixtures.size(); i++) {
            auto position = std::find(affectedFixtures_.begin(), affectedFixtures_.end(), fixtures[i]);
            bool actived = position != affectedFixtures_.end();
            bool nextActived = GuiToggle((Rectangle) {1500, 600 + (float)i * 20, 420, 20}, fixtures[i]->name_.c_str(), actived);
            bool clicked = actived != nextActived;
            if(clicked) {
                if(position != affectedFixtures_.end())
                    affectedFixtures_.erase(position);
                else
                    affectedFixtures_.push_back(fixtures[i]);
            }
        }
        DisplayValue(&innerRadius_, "innerRadius", (Vector2) {1500, 340}, 0, 1080);
        DisplayValue(&outerRadius_, "outerRadius", (Vector2) {1500, 380}, 0, 1080);

        if (GuiDropdownBox((Rectangle) {1500, 320, 420, 20}, "Addition;Subtract", &blendModeGui_, activeProp == &blendMode_)){
            blendMode_ = (Blend)blendModeGui_;
            if (activeProp == &blendMode_)
                activeProp = nullptr;
            else
                activeProp = &blendMode_;
        }

        GuiLabel((Rectangle) {1500, 600, 100, 20}, TextFormat("%d", affectedFixtures_.size()));
    }

    void Draw() override {
        DrawRectangleRec(GetSelector(), color_);
        DrawCircleLines(position_.x, position_.y, innerRadius_, enabled_ ? color_ : BLACK);
        DrawCircleLines(position_.x, position_.y, outerRadius_, enabled_ ? color_ : BLACK);
        DrawText(name_.c_str(), position_.x, position_.y, 20, selectedModifiable == this ? GREEN : BLUE);
    }
} follow;

class EmptyModifiable : public Modifiable {
    int selectedFixture_ = 0;
    int selectedModifier_ = 0;
public:
    void DrawProps() override {
        if(GuiButton((Rectangle) {1500, 20, 100, 20}, "Spawn Fixture")){
            fixtures.push_back(new Fixture((Vector2){500, 500}, (Vector2){500, 500}, "Fixture", 0, Fixture::Mode::Dimmer));
            selectedModifiable = fixtures[fixtures.size() - 1];
        }
        if(GuiButton((Rectangle) {1600, 20, 100, 20}, "Spawn Rectangle")){
            RectangleModifier* r = new RectangleModifier();
            modifiers.push_back(r);
            r->name_ = "Rectangle";
            selectedModifiable = r;
            
        }
        if(GuiButton((Rectangle) {1700, 20, 100, 20}, "Spawn Spot")){
            SpotModifier* s = new SpotModifier();
            modifiers.push_back(s);
            s->name_ = "Spot";
            selectedModifiable = s;
        }
        
        std::string fixturesNames;
        for(unsigned int i = 0; i < fixtures.size(); i++) {
            fixturesNames += fixtures[i]->name_;
            fixturesNames += ";";
        }
        fixturesNames.erase(fixturesNames.end() - 1);

        if (!fixtures.empty()) {
            if (GuiButton((Rectangle){1800, 40, 60, 20}, "Select"))
                selectedModifiable = fixtures[selectedFixture_];
            if (GuiButton((Rectangle){1860, 40, 60, 20}, "Delete"))
                fixtures.erase(fixtures.begin() + selectedFixture_); 
        }
        std::string modifiersNames;
        for(unsigned int i = 0; i < modifiers.size(); i++) {
            modifiersNames += modifiers[i]->name_;
            modifiersNames += ";";
        }
        modifiersNames.erase(modifiersNames.end() - 1);

        if (!modifiers.empty()) {
            if (GuiButton((Rectangle){1800, 60, 60, 20}, "Select"))
                selectedModifiable = modifiers[selectedModifier_];
            if (GuiButton((Rectangle){1860, 60, 60, 20}, "Delete")) {
                auto m = modifiers.begin() + selectedModifier_;
                delete (*m);
                modifiers.erase(m); 
            }
        }
        
        follow.DrawProps();

        if (GuiDropdownBox((Rectangle) {1500, 60, 300, 20}, modifiersNames.c_str(), &selectedModifier_, activeProp == &selectedModifier_)) {
            if (activeProp == &selectedModifier_)
                activeProp = nullptr;
            else
                activeProp = &selectedModifier_;
        }

        if (GuiDropdownBox((Rectangle) {1500, 40, 300, 20}, fixturesNames.c_str(), &selectedFixture_, activeProp == &selectedFixture_)) {
            if (activeProp == &selectedFixture_)
                activeProp = nullptr;
            else
                activeProp = &selectedFixture_;
       
        }
    };
};

EmptyModifiable e;

void Save() {
    std::vector<char> data;
    for(unsigned int i = 0; i < fixtures.size(); i++) {
        std::vector<char> v = fixtures[i]->Serialize();
        data.insert(data.end(), v.begin(), v.end());
        data.push_back('\n');
    }

    SaveFileData("data", &data[0], data.size());
}

void Load() {
    for(Fixture* f: fixtures)
        delete f;
    fixtures.clear();
    unsigned int dataSize = 0;
    unsigned char* data = LoadFileData("data", &dataSize);
    unsigned char* ptrInFile = data;
    while (ptrInFile - data < dataSize) {
        Fixture* f = new Fixture(ptrInFile);
        fixtures.push_back(f);
        ptrInFile += sizeof(Fixture);
        ptrInFile += std::strlen((char*)ptrInFile);
        ptrInFile += 2;
    }
    selectedModifiable = &e;
}

void SelectPosition(){
    *positionSelector = GetMousePosition();
    positionSelector = nullptr;
}

void MouseLeftClick(){
    if(IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
        if (GetMousePosition().x > 1500) return;
        if (positionSelector != nullptr) {
            SelectPosition();
            return;
        }
        activeProp = nullptr;
        selectedModifiable = &e;
        unsigned int i = 0;
        for(unsigned int i = 0; i < fixtures.size(); i++) {
            if(CheckCollisionPointCircle(GetMousePosition(), fixtures[i]->position_, 10)) {
                selectedModifiable = fixtures[i];
                break;
            }
        }
        for(unsigned int i = 0; i < modifiers.size(); i++) {
            if(CheckCollisionPointRec(GetMousePosition(), modifiers[i]->GetSelector())) {
                selectedModifiable = modifiers[i];
                break;
            }
        }
    }
}

bool debug = false;

ola::DmxBuffer buff(dmxValues, 512);

void SendDmx() {
    buff.Set(dmxValues, 512);
    dmxClient.SendDmx(0, buff);
}

void Init() {
    for (unsigned int i = 0; i < dmxAddresses; i++) 
        dmxValues[i] = 0;
    dmxClient.Setup();
    SendDmx();
    
    selectedModifiable = &e;

    InitWindow(1920, 1080, "raylib");

    SetTargetFPS(60);

    GuiLoadStyleDark();
}

ColorInt BlendFixtureWithModifier(Fixture f, Modifier* m) {
    switch (m->blendMode_){
        case (Modifier::Blend::Addition):
            return f.color_ + ColorMult(
                m->color_, 
                m->AmountWithLine(f.position_, f.lookingAt_)
            );
        case (Modifier::Blend::Subtract):
            return f.color_ - ColorMult(
                m->color_, 
                m->AmountWithLine(f.position_, f.lookingAt_)
            );
        default:
            return f.color_;
    } 
}

void Update() {
    for (unsigned int i = 0; i < dmxAddresses; i++) 
        dmxValues[i] = 0;
 
    for(unsigned int i = 0; i < fixtures.size(); i++)
        fixtures[i]->color_ = BLACK;

    for(unsigned int i = 0; i < follow.affectedFixtures_.size(); i++)
        fixtures[i]->color_ = BlendFixtureWithModifier(*follow.affectedFixtures_[i], &follow);
 
    for(unsigned int i = 0; i < modifiers.size(); i++) {
        for(unsigned int j = 0; j < modifiers[i]->affectedFixtures_.size(); j++) {
            Fixture* f = modifiers[i]->affectedFixtures_[j];
            f->color_ = BlendFixtureWithModifier(*f, modifiers[i]);
        }
    }
    if(IsKeyPressed(KEY_ENTER))
        activeProp = nullptr;
    if(activeProp == nullptr) {
        if(IsKeyPressed(KEY_S))
            Save();
        if(IsKeyPressed(KEY_L))
            Load();
        if(IsKeyPressed(KEY_F))
            follow.enabled_ = !follow.enabled_;
        if(IsKeyPressed(KEY_TAB))
            debug = !debug;
    }
    MouseLeftClick();
    if(follow.enabled_)
        follow.position_ = GetMousePosition();
    for(unsigned int i = 0; i < fixtures.size(); i++)
        fixtures[i]->WriteDmx();
    SendDmx();
}

void Draw() {
    ClearBackground((Color) {10,10,10,255});

    if(debug) {
        for(unsigned int i = 0; i < 32; i++)
            for(unsigned int j = 0; j < 16; j++)
                GuiLabel((Rectangle){(float)j * 40, (float)i * 20, 40, 20}, TextFormat("%d", dmxValues[i* 16 + j]));
    }

    for(unsigned int i = 0; i < fixtures.size(); i++) {
        fixtures[i]->Draw();
    }
    for(unsigned int i = 0; i < modifiers.size(); i++) {
        modifiers[i]->Draw();
    }

    follow.Draw();

    GuiPanel((Rectangle) {1500, 0, 420, 1080}, "Properties");
    if(positionSelector != nullptr) {
        GuiLabel((Rectangle) {1500, 500, 420, 20}, "Choose a position");
    }
    else {
        selectedModifiable->DrawProps();
        if(GuiButton((Rectangle){1500, 1060, 420, 20}, "Deselect"))
            selectedModifiable = &e;
    }
}

int main() {
    Init();
    while(!WindowShouldClose()) {
        Update();
        BeginDrawing();
        Draw();
        EndDrawing();
    }
}