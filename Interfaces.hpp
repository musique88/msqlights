#pragma once
#include <vector>
#include <rapidjson/document.h>

namespace MsqLights {
    class Engine;

    class Serializable {
    public:
        virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) = 0;
    };
    
    class Modifiable {
    protected:
        Engine* engine_;
    public:
        virtual void DrawProps() = 0;
        Modifiable(Engine* e) {
            engine_ = e;
        }
    };
    
    class Drawable {
    public:
        virtual void Draw() = 0;
    };
}