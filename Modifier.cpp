#include "Modifier.hpp"
#include "Engine.hpp"
#include "JsonHelper.hpp"

namespace MsqLights {
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
    }

    void Modifier::DrawProps() {
        engine_->DisplayValue(&name_, "name", (Vector2){1500, 20});
        if (GuiDropdownBox((Rectangle) {1500, 60, 420, 20}, "Addition;Subtract", &blendModeGui_, engine_->activeProp == &blendMode_)){
            blendMode_ = (Blend)blendModeGui_;
            if (engine_->activeProp == &blendMode_)
                engine_->activeProp = nullptr;
            else
                engine_->activeProp = &blendMode_;
        }

        color_ = GuiColorPicker((Rectangle){1520, 500, 370, 100}, "Color", color_);
        DrawRectangleRec((Rectangle) {1500, 500, 20, 100}, color_);
        for(unsigned int i = 0; i < engine_->fixtures.size(); i++) {
            auto position = std::find(affectedFixtures_.begin(), affectedFixtures_.end(), engine_->fixtures[i]);
            bool actived = position != affectedFixtures_.end();
            bool nextActived = GuiToggle((Rectangle) {1500, 600 + (float)i * 20, 420, 20}, engine_->fixtures[i]->name_.c_str(), actived);
            bool clicked = actived != nextActived;
            if(clicked) {
                if(!nextActived)
                    affectedFixtures_.erase(position);
                else
                    affectedFixtures_.push_back(engine_->fixtures[i]);
            }
        }

        GuiLabel((Rectangle) {1500, 600, 100, 20}, TextFormat("%d", affectedFixtures_.size()));
    }

    void Modifier::Update() {

    }

    void Modifier::Draw() {

    }

    Rectangle Modifier::GetSelector() {
        return (Rectangle) {0, 0, 0, 0};
    }
}