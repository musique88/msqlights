#pragma once
#include <vector>
#include <rapidjson/document.h>

namespace MsqLights {
    class Engine;

    class Serializable {
    public:
        virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) = 0;
    };
   
    class Drawable {
    public:
        virtual void Draw() = 0;
    };
}