#include "Follow.hpp"
#include "Engine.hpp"

namespace MsqLights {
    Follow::Follow(Engine* e) 
    : SpotModifier(e) {
        enabled_ = false;
        followingMouse_ = false;
    }

    void Follow::DrawProps() {
        GuiLabel((Rectangle) {1500, 300, 420, 20}, "Follow (Press f to toggle)");
        DrawRectangleRec((Rectangle) {1700, 300, 10, 10}, enabled_ ? GREEN : RED);
        color_ = GuiColorPicker((Rectangle){1520, 500, 370, 100}, "Color", color_);
        DrawRectangleRec((Rectangle) {1500, 500, 20, 100}, color_);
        for(unsigned int i = 0; i < engine_->fixtures.size(); i++) {
            auto position = std::find(affectedFixtures_.begin(), affectedFixtures_.end(), engine_->fixtures[i]);
            bool actived = position != affectedFixtures_.end();
            bool nextActived = GuiToggle((Rectangle) {1500, 600 + (float)i * 20, 420, 20}, engine_->fixtures[i]->name_.c_str(), actived);
            bool clicked = actived != nextActived;
            if(clicked) {
                if(position != affectedFixtures_.end())
                    affectedFixtures_.erase(position);
                else
                    affectedFixtures_.push_back(engine_->fixtures[i]);
            }
        }
        engine_->DisplayValue(&innerRadius_, "innerRadius", (Vector2) {1500, 340}, 0, 1080);
        engine_->DisplayValue(&outerRadius_, "outerRadius", (Vector2) {1500, 380}, 0, 1080);

        if (GuiDropdownBox((Rectangle) {1500, 320, 420, 20}, "Addition;Subtract", &blendModeGui_, engine_->activeProp == &blendMode_)){
            blendMode_ = (Blend)blendModeGui_;
            if (engine_->activeProp == &blendMode_)
                engine_->activeProp = nullptr;
            else
                engine_->activeProp = &blendMode_;
        }

        GuiLabel((Rectangle) {1500, 600, 100, 20}, TextFormat("%d", affectedFixtures_.size()));
 
    }

    void Follow::Draw() {
        DrawRectangleRec(GetSelector(), color_);
        DrawCircleLines(position_.x, position_.y, innerRadius_, enabled_ ? color_ : BLACK);
        DrawCircleLines(position_.x, position_.y, outerRadius_, enabled_ ? color_ : BLACK);
        DrawText(name_.c_str(), position_.x, position_.y, 20, engine_->selectedModifiable == this ? GREEN : BLUE);
    }
}