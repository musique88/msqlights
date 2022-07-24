#pragma once
#include <raylib.h>
#include "raygui.h"
#include "Interfaces.hpp"
#include <ola/DmxBuffer.h>
#include <ola/client/StreamingClient.h>
#include "Fixture.hpp"
#include "Modifier.hpp"
#include "RectangleModifier.hpp"
#include "SpotModifier.hpp"
#include "Follow.hpp"

namespace MsqLights {
    const unsigned int DMX_ADDRESSES = 512;

    class Engine {
    public:
        class EmptyModifiable : public Modifiable {
            int selectedFixture_;
            int selectedModifier_;
        public:
            EmptyModifiable(Engine* engine);
            void DrawProps() override;
        };

        unsigned char dmxValues[DMX_ADDRESSES];
        std::vector<Fixture*> fixtures;
        std::vector<Modifier*> modifiers;
        Modifiable* selectedModifiable;
        void* activeProp;
        Vector2* positionSelector;
        Follow follow;
        EmptyModifiable emptyModifiable;
        bool debug;
        ola::DmxBuffer dmxBuffer;
        ola::client::StreamingClient dmxClient;

        Engine();
        
        void DisplayValue(int* value, std::string propName, Vector2 pos, int min, int max);
        void DisplayValue(float* value, std::string propName, Vector2 pos, int min, int max);
        void DisplayValue(std::string* value, std::string propName, Vector2 pos);

        void DisplayPositionSelector(Vector2* position, std::string propName, Vector2 pos);
        void SelectPosition();
        void MouseLeftClick();
        void SendDmx();
        void DeselectModifiable();

        void Save();
        void Load();

        void Init();
        void Update();
        void Draw();
        void Close();
        void Loop();
    };
}