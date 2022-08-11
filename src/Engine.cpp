#include "Engine.hpp"
#include <cstring>
#include "dark.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

namespace MsqLights {
    Engine::EmptyModifiable::EmptyModifiable(Engine* engine)
    : Modifiable(engine) {
        selectedFixture_ = 0;
        selectedModifier_ = 0;
    }

    Engine::Engine() 
    : emptyModifiable(this), oscServer(this), 
    random(std::chrono::system_clock::now().time_since_epoch().count()) {
        selectedModifiable = &emptyModifiable;
        activeProp = nullptr;
        debug = false;
        positionSelector = nullptr;
        selectedPage = 0;
    }

    void Engine::SendDmx() {
        dmxBuffer.Set(dmxValues, 512);
        dmxClient.SendDmx(0, dmxBuffer);
    }

    void Engine::DeselectModifiable() {
        selectedModifiable = &emptyModifiable;
    }

    // REDO
    void Engine::Save() {
        rapidjson::Document d;
        d.SetObject();
        rapidjson::Value fixturesArray;
        fixturesArray.SetArray();
        rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
        for(unsigned int i = 0; i < fixtures.size(); i++)
            fixturesArray.PushBack(fixtures[i], allocator);
        d.AddMember("fixtures", fixturesArray, allocator);
        rapidjson::Value allTheModifiables;
        allTheModifiables.SetArray();
        for(auto &[key, val]: modifiables)
            allTheModifiables.PushBack(val->Serialize(allocator), allocator);
        
        d.AddMember("modifiers", allTheModifiables, allocator);

        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
        std::ofstream file("data.json");
        d.Accept(writer);
        file << sb.GetString();
    }

    void Engine::Load() {
        fixtures.clear();

        std::ifstream file("data.json");
        std::stringstream ss;
        ss << file.rdbuf();
        rapidjson::Document d;
        d.Parse(ss.str().c_str());

        auto fixturesArray = d["fixtures"].GetArray();

        auto allTheModifiers = d["modifiers"].GetArray();
        for(unsigned int i = 0; i < allTheModifiers.Size(); i++) {
            auto page = allTheModifiers[i].GetArray(); 
            for(unsigned int j = 0; j < page.Size(); j++) {
                auto mod = page[j].GetObject();
                const char* typeStr = mod["type"].GetString();

                Modifier* modif = nullptr;
                if(!strcmp(typeStr, "Rectangle"))
                    modif = new GlobalModifier(this);
                else if(!strcmp(typeStr, "Spot"))
                    modif = new SpotModifier(this);
                modif->Init();
                modif->Load(page[j]);
            }
        }
    }

    void Engine::Init() {
        for (unsigned int i = 0; i < DMX_ADDRESSES; i++) 
            dmxValues[i] = 0;
        dmxClient.Setup();
        oscServer.Start();
        SendDmx();
    
        DeselectModifiable();
    }

    void Engine::Update() {
        for (unsigned int i = 0; i < DMX_ADDRESSES; i++) 
            dmxValues[i] = 0;
 
        if(IsKeyPressed(KEY_ENTER))
            activeProp = nullptr;
        if(activeProp == nullptr) {
            if(IsKeyPressed(KEY_S))
                Save();
            if(IsKeyPressed(KEY_L))
                Load();
            if(IsKeyPressed(KEY_TAB))
                debug = !debug;
        }
        SendDmx();
        oscServer.Update();

        for(auto&[_,val]: modifiables)
            val->Update();
    }

    void Engine::Close() {
        oscServer.Close();
    }

    void Engine::Loop() {
        Init();
        while(true)
            Update();
        Close();
    }
}