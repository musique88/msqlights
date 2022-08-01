#include "RectangleModifier.hpp"
#include "Engine.hpp"
#include "JsonHelper.hpp"

namespace MsqLights {

    RectangleModifier::RectangleModifier(Engine* e)
    : Modifier(e) {
        type_ = "Rectangle";
        rect_ = {0,0,0,0};
    }

    void RectangleModifier::DrawProps() {
        engine_->DisplayValue(&rect_.x, "rect.x", (Vector2) {WIDTH - PANELSIZE, 80}, 0, 1500);
        engine_->DisplayValue(&rect_.y, "rect.y", (Vector2) {WIDTH - PANELSIZE, 120}, 0, 1080);
        engine_->DisplayValue(&rect_.width, "rect.w", (Vector2) {WIDTH - PANELSIZE, 160}, 0, 1500);
        engine_->DisplayValue(&rect_.height, "rect.h", (Vector2) {WIDTH - PANELSIZE, 200}, 0, 1080);
        engine_->DisplayPositionSelector((Vector2*)&rect_, "Origin", (Vector2) {WIDTH - PANELSIZE, 240});
        Modifier::DrawProps();
    } 

    void RectangleModifier::Draw() {
        DrawRectangleRec(GetSelector(), color_);
        DrawRectangleLinesEx(rect_, 3, color_);
        DrawText(name_.c_str(), rect_.x, rect_.y, 20, engine_->selectedModifiable == this ? GREEN : BLUE);
    }


    Rectangle RectangleModifier::GetSelector() {
        return (Rectangle) {rect_.x - 10, rect_.y - 10, 20, 20};
    }

    float RectangleModifier::AmountWithLine(Vector2 a, Vector2 b) {
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

    void RectangleModifier::SetPosition(Vector2 p) {
        rect_.x = p.x;
        rect_.y = p.y;
    } 

    void RectangleModifier::RegisterParameters() {
        Modifier::RegisterParameters();
        parameters_.emplace("rect.x", &rect_.x);
        parameters_.emplace("rect.y", &rect_.y);
        parameters_.emplace("rect.w", &rect_.width);
        parameters_.emplace("rect.h", &rect_.height);
    }
}