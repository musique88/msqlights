#include "CircleSpotModifier.hpp"
#include "Engine.hpp"
#include <cmath>
#include "JsonHelper.hpp"

namespace MsqLights {
    float Distance(Vector2 v) {
        return sqrt(v.x * v.x + v.y * v.y);
    }


    void CircleSpotModifier::SetParam(std::string paramname, float val) {
        Modifier::SetParam(paramname, val);
        if(paramname == "pos.x")
            origin_.x = val;
        else if(paramname == "pos.y")
            origin_.y = val;
        else if(paramname == "innerRadius")
            innerRadius_ = val;
        else if(paramname == "outerRadius")
            outerRadius_ = val;
        else if(paramname == "speed")
            speed_ = val;
        else if(paramname == "phase")
            phase_ = val;
        else if(paramname == "size.x")
            size_.x = val;
        else if(paramname == "size.y")
            size_.y = val;
    }

    rapidjson::Value CircleSpotModifier::Serialize(rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value val;
        val = SpotModifier::Serialize(allocator);
        val["type"] = "CircleSpot";
        val.AddMember("origin", MsqLights::Serialize(origin_, allocator), allocator);
        val.AddMember("size", MsqLights::Serialize(size_, allocator), allocator);
        val.AddMember("speed", speed_, allocator);
        val.AddMember("phase", phase_, allocator);
        return val;
    } 

    CircleSpotModifier::CircleSpotModifier(Engine* e, rapidjson::Value& val) 
    : SpotModifier(e, val) {
        origin_ = Vector2Parse(val["origin"]);
        size_ = Vector2Parse(val["size"]);
        speed_ = val["speed"].GetFloat();
        phase_ = val["phase"].GetFloat();
    }

    CircleSpotModifier::CircleSpotModifier(Engine* e) 
    : SpotModifier(e) {
        type_ = "CircleSpot";
        origin_ = (Vector2) {0,0};
        size_ = (Vector2) {0,0};
        speed_ = 0.10f;
        phase_ = 0.f;
        currentPhase_ = 0.f;
    }

    void CircleSpotModifier::DrawProps() {
        engine_->DisplayValue(&origin_.x, "pos.x", (Vector2) {WIDTH - PANELSIZE, 80}, 0, 1500);
        engine_->DisplayValue(&origin_.y, "pos.y", (Vector2) {WIDTH - PANELSIZE, 120}, 0, 1080);
        engine_->DisplayValue(&innerRadius_, "innerRadius", (Vector2) {WIDTH - PANELSIZE, 160}, 0, 1080);
        engine_->DisplayValue(&outerRadius_, "outerRadius", (Vector2) {WIDTH - PANELSIZE, 200}, 0, 1080);
        engine_->DisplayPositionSelector(&origin_, "Origin", (Vector2) {WIDTH - PANELSIZE, 240});

        float speed = speed_ * 1000;
        engine_->DisplayValue(&speed, "speed (kHz)", (Vector2) {WIDTH - PANELSIZE, 280}, -1000, 1000);
        speed_ = speed / 1000;

        float phase = phase_ * 100;
        engine_->DisplayValue(&phase, "phase (%)", (Vector2) {WIDTH - PANELSIZE, 320}, 0, 100);
        phase_ = phase / 100;

        engine_->DisplayValue(&size_.x, "size.x", (Vector2) {WIDTH - PANELSIZE, 360}, 0, 1500);
        engine_->DisplayValue(&size_.y, "size.y", (Vector2) {WIDTH - PANELSIZE, 400}, 0, 1080);
        
        Modifier::DrawProps();
    }

    void CircleSpotModifier::Update() {
        position_.x = origin_.x + (size_.x * sin(2 * M_PI * (currentPhase_ + phase_)));
        position_.y = origin_.y + (size_.y * cos(2 * M_PI * (currentPhase_ + phase_)));
        currentPhase_ += speed_ * GetFrameTime();
        currentPhase_ = currentPhase_ - (int)currentPhase_;
    }

    void CircleSpotModifier::Draw() {
        DrawRectangleRec(GetSelector(), color_);
        DrawCircle(position_.x, position_.y, 10, BLUE);
        DrawCircleLines(position_.x, position_.y, innerRadius_, color_);
        DrawCircleLines(position_.x, position_.y, outerRadius_, color_);
        DrawEllipseLines(origin_.x, origin_.y, size_.x, size_.y, color_);
        DrawText(name_.c_str(), origin_.x, origin_.y, 20, engine_->selectedModifiable == this ? GREEN : BLUE);
    }

    Rectangle CircleSpotModifier::GetSelector() {
        return (Rectangle) {origin_.x - 10, origin_.y - 10, 20, 20};
    }

    float CircleSpotModifier::AmountWithLine(Vector2 a, Vector2 b) {
        return SpotModifier::AmountWithLine(a, b);
    }
}