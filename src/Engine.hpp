#pragma once
#include "Interfaces.hpp"
#include <ola/DmxBuffer.h>
#include <ola/client/ClientWrapper.h>
#include <ola/io/SelectServer.h>
#include <ola/Callback.h>
#include "Fixture.hpp"
#include "Modifier.hpp"
#include "GlobalModifier.hpp"
#include "SpotModifier.hpp"
#include "OscServer.hpp"
#include <random>
#include <map>
#include <chrono>
#include <thread>
#include <vector>
#include <queue>
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <utility>

#define WIDTH 1366
#define HEIGHT 768 
#define PANELSIZE 420

namespace MsqLights {
    const unsigned int DMX_ADDRESSES = 512;
    const unsigned int PAGES = 16;

    class Engine {
        float deltaTime;
        std::chrono::time_point<std::chrono::steady_clock> lastTime;
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
        ola::client::OlaClientWrapper dmxClient;
        OscServer oscServer;
        std::default_random_engine random;

        std::thread dmxIOThread;
        std::thread connectionAcceptorThread;
        std::vector<std::thread> connectionsThreads;

        std::queue<std::pair<std::string, int>> commandQueue;

        Engine();

        static bool SendDmx(Engine* e, ola::client::OlaClientWrapper* wrapper);
        static void DmxIO(Engine* e);
        static void ConnectionAcceptor(Engine* e);
        static void ConnectionManager(Engine* e, int socket, struct sockaddr clientinfo);
        
        float GetDeltaTime();

        void Save();
        void Load();

        std::vector<
            std::pair<
                std::string, 
                std::vector<std::string>
            >
        > SplitCommands(std::string str);
        void ExecuteCommand(std::string str, int socketFD);
        void Init();
        void Update();
        void Close();
        void Loop();
    };
}