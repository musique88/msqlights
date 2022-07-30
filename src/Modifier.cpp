#include "Modifier.hpp"
#include "Engine.hpp"
#include "JsonHelper.hpp"

namespace MsqLights {

    void Modifier::SetParam(std::string paramname, float val) {
        if(!params.count(paramname))
            return;
        params[paramname].setVal(val);
    }

    rapidjson::Value Modifier::Serialize(rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value val;
        val.SetObject();
        rapidjson::Value name;
        name.SetString(name_.c_str(), allocator);
        val.AddMember("name", name, allocator);
        val.AddMember("blendMode", (int)blendMode_, allocator);
        val.AddMember("color", MsqLights::Serialize(color_, allocator), allocator);
        rapidjson::Value affectedFixturesArray;
        affectedFixturesArray.SetArray();
        for(unsigned int i = 0; i < affectedFixtures_.size(); i++) {
            affectedFixturesArray.PushBack(
                std::distance( 
                    engine_->fixtures.begin(), 
                    std::find(
                        engine_->fixtures.begin(), 
                        engine_->fixtures.end(), 
                        affectedFixtures_[i]
                    )
                ),
                allocator
            );
        }
        val.AddMember("affectedFixtures", affectedFixturesArray, allocator);
        return val;
    } 


    Modifier::Modifier(Engine* engine, rapidjson::Value& val) 
    : Modifiable(engine) {
        name_ = val["name"].GetString();
        blendMode_ = (Blend)val["blendMode"].GetInt();
        color_ = ColorParse(val["color"]);
        auto affectedFixturesArr = val["affectedFixtures"].GetArray();
        for(unsigned int i = 0; i < affectedFixturesArr.Size(); i++)
            affectedFixtures_.push_back(engine_->fixtures[affectedFixturesArr[i].GetInt()]);
    }

    Modifier::Modifier(Engine* engine) 
    : Modifiable(engine) {
        blendMode_ = Blend::Addition;
        color_ = {0,0,0,255};
        type_ = "None";
        fade_ = 0;
    }

    void Modifier::DrawProps() {
        engine_->DisplayValue(&name_, "name", (Vector2){WIDTH - PANELSIZE, 20});
        if (GuiDropdownBox((Rectangle) {WIDTH - PANELSIZE, 60, 420, 20}, "Addition;Subtract", &blendModeGui_, engine_->activeProp == &blendMode_)){
            blendMode_ = (Blend)blendModeGui_;
            if (engine_->activeProp == &blendMode_)
                engine_->activeProp = nullptr;
            else
                engine_->activeProp = &blendMode_;
        }
        fade_ = GuiSlider((Rectangle){WIDTH - PANELSIZE + 80, 480, PANELSIZE - 80, 20}, 
            TextFormat("Fade %2.3f", fade_), "", fade_, 0, 10 
        );
        tempColor_ = GuiColorPicker((Rectangle){WIDTH - PANELSIZE + 20, 500, 370, 100}, "Color", tempColor_);
        DrawRectangleRec((Rectangle) {WIDTH - PANELSIZE, 500, 20, 40}, color_);
        DrawRectangleRec((Rectangle) {WIDTH - PANELSIZE, 540, 20, 20}, nextColor_);
        DrawRectangleRec((Rectangle) {WIDTH - PANELSIZE, 560, 20, 20}, tempColor_);
        if (GuiButton((Rectangle) {WIDTH - PANELSIZE, 580, 20, 20}, "GO")) {
            oldColor_ = color_;
            nextColor_ = tempColor_;
            fadeTimer_ = 0;
            fadeDone_ = false;
        }
        engine_->DisplaySelectedFixtures(&affectedFixtures_, (Vector2){WIDTH - PANELSIZE, 600});
    }

    void Modifier::Update() {
        fadeTimer_ += GetFrameTime();
        if (fadeDone_)
            return;
        if (fade_ == 0) {
            color_ = nextColor_;
            return;
        }
        if (fadeTimer_ / fade_ > 1) {
            fadeDone_ = true;
            color_ = nextColor_;
            return;
        }
        color_ = ColorFade(nextColor_, oldColor_, fadeTimer_ / fade_);
    }

    void Modifier::Draw() {

    }

    Rectangle Modifier::GetSelector() {
        return (Rectangle) {0, 0, 0, 0};
    }

    Modifier::~Modifier() {

    }

    void Modifier::RegisterParams() {
        params.emplace("blendMode", (unsigned char*)(&blendMode_));
        params.emplace("color.r", &color_.r);
        params.emplace("color.g", &color_.g);
        params.emplace("color.b", &color_.b);
        params.emplace("fade", &fade_);
    }

    void Modifier::Init() {
        RegisterParams();
    }
}