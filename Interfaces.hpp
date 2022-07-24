#pragma once
#include <vector>

namespace MsqLights {
    class Engine;

    class Serializable {
    public:
        virtual std::vector<char> Serialize() = 0;
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