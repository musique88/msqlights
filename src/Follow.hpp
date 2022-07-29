#pragma once
#include "SpotModifier.hpp"

namespace MsqLights {
    class Follow : public SpotModifier {
    public:
        bool enabled_;
        bool followingMouse_;

        Follow(Engine* e);

        void DrawProps() override;
        void Draw() override;
    };
}