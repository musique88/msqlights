#pragma once
#include <raylib.h>
#include <raygui.h>
#include "Interfaces.hpp"
#include <ola/DmxBuffer.h>
#include <ola/client/StreamingClient.h>
#include "Fixture.hpp"
#include "Modifier.hpp"
#include "GlobalModifier.hpp"
#include "SpotModifier.hpp"
#include "OscServer.hpp"
#include <random>
#include <map>

#define WIDTH 1366
#define HEIGHT 768 
#define PANELSIZE 420

namespace MsqLights {
    const unsigned int DMX_ADDRESSES = 512;
    const unsigned int PAGES = 16;

    class Engine {
    public:
        class EmptyModifiable : public Modifiable {
            int selectedFixture_;
            int selectedModifier_;
            float fade_;
        
        public:
            EmptyModifiable(Engine* engine);
        };

        unsigned char dmxValues[DMX_ADDRESSES];
        std::vector<unsigned int> fixtures;
        std::vector<std::vector<unsigned int>> scenes;
        std::map<unsigned int, Modifiable*> modifiables;
        unsigned int selectedPage;
        Modifiable* selectedModifiable;
        void* activeProp;
        Vector2* positionSelector;
        EmptyModifiable emptyModifiable;
        bool debug;
        ola::DmxBuffer dmxBuffer;
        ola::client::StreamingClient dmxClient;
        OscServer oscServer;
        std::default_random_engine random;

        Engine();
        
        void DisplayValue(int* value, std::string propName, Vector2 pos, int min, int max);
        void DisplayValue(float* value, std::string propName, Vector2 pos, int min, int max);
        void DisplayValue(std::string* value, std::string propName, Vector2 pos);
        void DisplayPositionSelector(Vector2* position, std::string propName, Vector2 pos);
        void DisplaySelectedFixtures(std::vector<Fixture*>* selectedFixtures, Vector2 pos);

        void SendDmx();
        void DeselectModifiable();

        void Save();
        void Load();

        void Init();
        void Update();
        void Close();
        void Loop();
    };
}