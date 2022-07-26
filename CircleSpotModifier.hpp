#include "SpotModifier.hpp"

namespace MsqLights {
    class CircleSpotModifier : public SpotModifier {
    public:
        Vector2 origin_;
        Vector2 size_;
        float speed_;
        float phase_;
        float currentPhase_;

        CircleSpotModifier(Engine* e);
        CircleSpotModifier(Engine* e, rapidjson::Value& val);

        virtual void SetParam(std::string paramname, float val) override;
        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;
        void DrawProps() override;
        void Update() override;
        void Draw() override;
        Rectangle GetSelector() override;
        float AmountWithLine(Vector2 a, Vector2 b) override;
    };
}
