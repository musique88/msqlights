#include "SpotModifier.hpp"

namespace MsqLights {
    class LineSpotModifier : public SpotModifier {
    public:
        Vector2 origin_;
        Vector2 destination_;
        float speed_;
        float phase_;
        float currentPhase_;

        LineSpotModifier(Engine* e);

        void DrawProps() override;
        void Update() override;
        void Draw() override;
        Rectangle GetSelector() override;
        float AmountWithLine(Vector2 a, Vector2 b) override;

        virtual void RegisterParameters() override;
    };
}
