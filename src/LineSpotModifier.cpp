#include "LineSpotModifier.hpp"
#include "Engine.hpp"
#include "JsonHelper.hpp"

namespace MsqLights {
    LineSpotModifier::LineSpotModifier(Engine* e, rapidjson::Value& val)
    : SpotModifier(e, val) {
        origin_ = Vector2Parse(val["origin"]);
        destination_ = Vector2Parse(val["destination"]);
        speed_ = val["speed"].GetFloat();
        phase_ = val["phase"].GetFloat();
    }

    LineSpotModifier::LineSpotModifier(Engine* e) 
    : SpotModifier(e) {
        type_ = "LineSpot";
        origin_ = (Vector2) {0,0};
        destination_ = (Vector2) {0,0};
        speed_ = 0.10f;
        phase_ = 0.f;
        currentPhase_ = 0.f;
    }

    rapidjson::Value LineSpotModifier::Serialize(rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value val;
        val = SpotModifier::Serialize(allocator);
        val["type"] = "LineSpot";
        val.AddMember("origin", MsqLights::Serialize(origin_, allocator), allocator);
        val.AddMember("destination", MsqLights::Serialize(destination_, allocator), allocator);
        val.AddMember("speed", speed_, allocator);
        val.AddMember("phase", phase_, allocator);
        return val;
    } 


    void LineSpotModifier::DrawProps() {
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

        engine_->DisplayValue(&destination_.x, "destination.x", (Vector2) {WIDTH - PANELSIZE, 360}, 0, 1500);
        engine_->DisplayValue(&destination_.y, "destination.y", (Vector2) {WIDTH - PANELSIZE, 400}, 0, 1080);
        engine_->DisplayPositionSelector(&destination_, "Destination", (Vector2) {WIDTH - PANELSIZE, 440});
        
        Modifier::DrawProps();
    }

    void LineSpotModifier::Update() {
        float phase = (currentPhase_ + phase_);
        int phaseInt = phase; 
        phase = phase - phaseInt;
        if(phase < 0)
            phase += 1;
        position_.x = origin_.x + ((destination_.x - origin_.x) * phase);
        position_.y = origin_.y + ((destination_.y - origin_.y) * phase);
        currentPhase_ += speed_ * GetFrameTime();
        currentPhase_ = currentPhase_ - (int)currentPhase_;
        Modifier::Update();
    }

    void LineSpotModifier::Draw() {
        DrawRectangleRec(GetSelector(), color_);
        DrawCircle(position_.x, position_.y, 10, BLUE);
        DrawCircleLines(position_.x, position_.y, innerRadius_, color_);
        DrawCircleLines(position_.x, position_.y, outerRadius_, color_);
        DrawLineV(origin_, destination_, color_);
        DrawText(name_.c_str(), origin_.x, origin_.y, 20, engine_->selectedModifiable == this ? GREEN : BLUE);
    }

    Rectangle LineSpotModifier::GetSelector() {
        return (Rectangle) {origin_.x - 10, origin_.y - 10, 20, 20};
    }

    float LineSpotModifier::AmountWithLine(Vector2 a, Vector2 b) {
        return SpotModifier::AmountWithLine(a, b);
    }

    void LineSpotModifier::RegisterParams() {
        SpotModifier::RegisterParams();
        params.insert({"pos.x", Parameter(&origin_.x)});
        params.insert({"pos.y", Parameter(&position_.y)});
        params.emplace("destination.x", &destination_.x);
        params.emplace("destination.y", &destination_.y);
        params.emplace("phase", &phase_);
        params.emplace("speed", &speed_);
    }
}
