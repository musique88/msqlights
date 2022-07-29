#include "Follow.hpp"
#include "Engine.hpp"

namespace MsqLights {
    Follow::Follow(Engine* e) 
    : SpotModifier(e) {
        enabled_ = false;
        followingMouse_ = false;
    }

    void Follow::DrawProps() {
        color_ = GuiColorPicker((Rectangle){WIDTH - PANELSIZE + 20, 10 * 20, 370, 100}, "Color", color_);
        DrawRectangleRec((Rectangle) {WIDTH - PANELSIZE, 10 * 20, 20, 100}, color_);

        tempColor_ = GuiColorPicker((Rectangle){WIDTH - PANELSIZE + 20, 370, 370, 100}, "Color", tempColor_);
        DrawRectangleRec((Rectangle) {WIDTH - PANELSIZE, 370, 20, 80}, color_);
        if (GuiButton((Rectangle) {WIDTH - PANELSIZE, 450, 20, 20}, "GO"))
            color_ = tempColor_;
        
        engine_->DisplaySelectedFixtures(&affectedFixtures_, (Vector2) {WIDTH - PANELSIZE, 15 * 20});
        engine_->DisplayValue(&innerRadius_, "innerRadius", (Vector2) {WIDTH - PANELSIZE, 5 * 20}, 0, 1080);
        engine_->DisplayValue(&outerRadius_, "outerRadius", (Vector2) {WIDTH - PANELSIZE, 7 * 20}, 0, 1080);

        if (GuiDropdownBox((Rectangle) {WIDTH - PANELSIZE, 4 * 20, 420, 20}, "Addition;Subtract", &blendModeGui_, engine_->activeProp == &blendMode_)){
            blendMode_ = (Blend)blendModeGui_;
            if (engine_->activeProp == &blendMode_)
                engine_->activeProp = nullptr;
            else
                engine_->activeProp = &blendMode_;
        }
    }

    void Follow::Draw() {
        DrawRectangleRec(GetSelector(), color_);
        DrawCircleLines(position_.x, position_.y, innerRadius_, enabled_ ? color_ : BLACK);
        DrawCircleLines(position_.x, position_.y, outerRadius_, enabled_ ? color_ : BLACK);
        DrawText(name_.c_str(), position_.x, position_.y, 20, engine_->selectedModifiable == this ? GREEN : BLUE);
    }
}