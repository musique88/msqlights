#include "Modifier.hpp"
#include "Engine.hpp"
#include "JsonHelper.hpp"

namespace MsqLights {

    void Modifier::SetParameter(std::string paramname, float val) {
        if(!parameters_.count(paramname))
            return;
        *std::get<float*>(parameters_[paramname]) = val;
    }

    rapidjson::Value Modifier::Serialize(rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value val = Modifiable::Serialize(allocator);

        rapidjson::Value affectedFixturesArray;
        affectedFixturesArray.SetArray();
        for(unsigned int i = 0; i < affectedFixtures_.size(); i++) {
            affectedFixturesArray.PushBack(affectedFixtures_[i], allocator);
        }
        val.AddMember("affectedFixtures", affectedFixturesArray, allocator);
        return val;
    } 

    Modifier::Modifier(Engine* engine) 
    : Modifiable(engine) {
        blendMode_ = Blend::Addition;
        color_ = {0,0,0,255};
        type_ = "None";
        fade_ = 0;
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

    Modifier::~Modifier() {

    }

    void Modifier::RegisterParameters() {
        Modifiable::RegisterParameters();
        parameters_.emplace("blendMode", (unsigned char*)(&blendMode_));
        parameters_.emplace("color.r", &color_.r);
        parameters_.emplace("color.g", &color_.g);
        parameters_.emplace("color.b", &color_.b);
        parameters_.emplace("fade", &fade_);
        parameters_.emplace("name", &name_);
        parameters_.emplace(std::string("type"), &type_);
    }

    void Modifier::Load(rapidjson::Value& val) {
        Modifiable::Load(val);

        auto affectedFixturesArr = val["affectedFixtures"].GetArray();
        for(unsigned int i = 0; i < affectedFixturesArr.Size(); i++)
            affectedFixtures_.push_back(engine_->fixtures[affectedFixturesArr[i].GetInt()]);
    }
}