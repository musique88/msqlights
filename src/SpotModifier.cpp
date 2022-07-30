#include "SpotModifier.hpp"
#include "Engine.hpp"
#include <algorithm>
#include <cmath>
#include "JsonHelper.hpp"

namespace MsqLights {
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


    rapidjson::Value SpotModifier::Serialize(rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value val;
        val = Modifier::Serialize(allocator);
        val.AddMember("type", "Spot", allocator);
        val.AddMember("position", MsqLights::Serialize(position_, allocator), allocator);
        val.AddMember("innerRadius", innerRadius_, allocator);
        val.AddMember("outerRadius", outerRadius_, allocator);
        return val;
    } 

    SpotModifier::SpotModifier(Engine* e, rapidjson::Value& val) 
    : Modifier(e, val){
        position_ = Vector2Parse(val["position"]);
        innerRadius_ = val["innerRadius"].GetFloat();
        outerRadius_ = val["outerRadius"].GetFloat();
    }

    SpotModifier::SpotModifier(Engine* e)
    : Modifier(e) {
        type_ = "Spot";
        innerRadius_ = 0;
        outerRadius_ = 0;
    }

    void SpotModifier::DrawProps() {
        engine_->DisplayValue(&position_.x, "pos.x", (Vector2) {WIDTH - PANELSIZE, 80}, 0, 1500);
        engine_->DisplayValue(&position_.y, "pos.y", (Vector2) {WIDTH - PANELSIZE, 120}, 0, 1080);
        engine_->DisplayValue(&innerRadius_, "innerRadius", (Vector2) {WIDTH - PANELSIZE, 160}, 0, 1080);
        engine_->DisplayValue(&outerRadius_, "outerRadius", (Vector2) {WIDTH - PANELSIZE, 200}, 0, 1080);
        engine_->DisplayPositionSelector(&position_, "Origin", (Vector2) {WIDTH - PANELSIZE, 240});
        Modifier::DrawProps();
    }

    void SpotModifier::Draw() {
        DrawRectangleRec(GetSelector(), color_);
        DrawCircleLines(position_.x, position_.y, innerRadius_, color_);
        DrawCircleLines(position_.x, position_.y, outerRadius_, color_);
        DrawText(name_.c_str(), position_.x, position_.y, 20, engine_->selectedModifiable == this ? GREEN : BLUE);
    }

    Rectangle SpotModifier::GetSelector() {
        return (Rectangle) {position_.x - 10, position_.y - 10, 20, 20};
    }

    float SpotModifier::AmountWithLine(Vector2 a, Vector2 b) {
        float dist = pDistance(position_, a, b);
        if (dist < innerRadius_)
            return 1.f;
        if (dist > outerRadius_)
            return 0.f;
        return 1.f - ((dist - innerRadius_) / (outerRadius_ - innerRadius_));
    }

    void SpotModifier::SetPosition(Vector2 p) {
        position_ = p;
    } 

    void SpotModifier::RegisterParams() {
        Modifier::RegisterParams();
        params.emplace("pos.x", &position_.x);
        params.emplace("pos.y", &position_.y);
        params.emplace("innerRadius", &innerRadius_);
        params.emplace("outerRadius", &outerRadius_);
    }
}