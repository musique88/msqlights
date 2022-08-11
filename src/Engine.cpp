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

    void Engine::EmptyModifiable::DrawProps() {
        if(GuiButton((Rectangle) {WIDTH - PANELSIZE, 20, 80, 20}, "Fixture")){
            engine_->fixtures.push_back(new Fixture(engine_, (Vector2){500, 500}, (Vector2){500, 500}, "Fixture", 0, Fixture::Mode::Dimmer));
            engine_->selectedModifiable = engine_->fixtures[engine_->fixtures.size() - 1];
        }
        if(GuiButton((Rectangle) {WIDTH - PANELSIZE + 80, 20, 80, 20}, "Rectangle")){
            GlobalModifier* r = new GlobalModifier(engine_);
            engine_->GetModifiers()->push_back(r);
            r->name_ = "Rectangle";
            engine_->selectedModifiable = r;
            r->Init();
        }
        if(GuiButton((Rectangle) {WIDTH - PANELSIZE + 160, 20, 80, 20}, "Spot")){
            SpotModifier* s = new SpotModifier(engine_);
            engine_->GetModifiers()->push_back(s);
            s->name_ = "Spot";
            engine_->selectedModifiable = s;
            s->Init();
        }
        
        
        std::string fixturesNames;
        for(unsigned int i = 0; i < engine_->fixtures.size(); i++) {
            fixturesNames += engine_->fixtures[i]->name_;
            fixturesNames += ";";
        }
        fixturesNames.erase(fixturesNames.end() - 1);

        if (!engine_->fixtures.empty()) {
            if (GuiButton((Rectangle){WIDTH - PANELSIZE + 300, 40, 60, 20}, "Select"))
                engine_->selectedModifiable = engine_->fixtures[selectedFixture_];
            if (GuiButton((Rectangle){WIDTH - PANELSIZE + 360, 40, 60, 20}, "Delete"))
                engine_->fixtures.erase(engine_->fixtures.begin() + selectedFixture_); 
        }
        std::string modifiersNames;
        for(unsigned int i = 0; i < engine_->GetModifiers()->size(); i++) {
            modifiersNames += engine_->GetModifiers()->at(i)->name_;
            modifiersNames += ";";
        }
        modifiersNames.erase(modifiersNames.end() - 1);

        if (!engine_->GetModifiers()->empty()) {
            if (GuiButton((Rectangle){WIDTH - PANELSIZE + 300, 60, 60, 20}, "Select"))
                engine_->selectedModifiable = engine_->GetModifiers()->at(selectedModifier_);
            if (GuiButton((Rectangle){WIDTH - PANELSIZE + 360, 60, 60, 20}, "Delete")) {
                auto m = engine_->GetModifiers()->begin() + selectedModifier_;
                delete (*m);
                engine_->GetModifiers()->erase(m); 
            }
        }
        
        engine_->follow.DrawProps();

        if (GuiDropdownBox((Rectangle) {WIDTH - PANELSIZE, 60, 300, 20}, modifiersNames.c_str(), &selectedModifier_, engine_->activeProp == &selectedModifier_)) {
            if (engine_->activeProp == &selectedModifier_)
                engine_->activeProp = nullptr;
            else
                engine_->activeProp = &selectedModifier_;
        }

        if (GuiDropdownBox((Rectangle) {WIDTH - PANELSIZE, 40, 300, 20}, fixturesNames.c_str(), &selectedFixture_, engine_->activeProp == &selectedFixture_)) {
            if (engine_->activeProp == &selectedFixture_)
                engine_->activeProp = nullptr;
            else
                engine_->activeProp = &selectedFixture_;
        }
        fade_ = GuiSlider((Rectangle){WIDTH - PANELSIZE + 80, HEIGHT - 60, PANELSIZE - 80, 20}, 
            TextFormat("Fade %2.3f", fade_), "", fade_, 0, 10 
        );
        for(unsigned int i = 0; i < PAGES; i++) {
            if (GuiButton((Rectangle) {(float)(WIDTH - PANELSIZE + i * 20), HEIGHT - 40, 20, 20}, TextFormat("%d", i))){
                engine_->selectedPage = i;
                for (unsigned int i = 0; i < engine_->fixtures.size(); i++)
                    engine_->fixtures[i]->BlendTo(fade_);
            }
        }

        GuiLabel((Rectangle) {(WIDTH - PANELSIZE) + 320, HEIGHT - 40, 100, 20}, TextFormat("PAGE: %d", engine_->selectedPage));
    }

    Engine::Engine() 
    : follow(this), emptyModifiable(this), oscServer(this), 
    random(std::chrono::system_clock::now().time_since_epoch().count()) {
        selectedModifiable = &emptyModifiable;
        activeProp = nullptr;
        debug = false;
        positionSelector = nullptr;
        selectedPage = 0;
    }

    std::vector<Modifier*>* Engine::GetModifiers() {
        return &modifiers[selectedPage];
    }

    void Engine::DisplayValue(int* value, std::string propName, Vector2 pos, int min, int max) {
        GuiLabel((Rectangle){pos.x, pos.y, 300, 20}, propName.c_str());
        *value = GuiSliderBar((Rectangle){pos.x, pos.y + 20, 420, 20}, nullptr, nullptr, *value, min, max);
        if (GuiValueBox((Rectangle){pos.x + 300, pos.y, 120, 20}, nullptr, value, min, max, activeProp == (void*)value)) {activeProp = (void*)value;};
    }

    void Engine::DisplayValue(float* value, std::string propName, Vector2 pos, int min, int max) {
        GuiLabel((Rectangle){pos.x, pos.y, 300, 20}, propName.c_str());
        *value = GuiSliderBar((Rectangle){pos.x, pos.y + 20, 420, 20}, nullptr, nullptr, *value, min, max);
        int val = *value;
        if (GuiValueBox((Rectangle){pos.x + 300, pos.y, 120, 20}, nullptr, &val, min, max, activeProp == (void*)value)) {activeProp = (void*)value;};
        *value = val;
    }

    void Engine::DisplayValue(std::string* value, std::string propName, Vector2 pos) {
        GuiLabel((Rectangle){pos.x, pos.y, 420, 20}, propName.c_str());
        char name[value->size() + 1];
        std::copy(value->begin(), value->end(), name);
        name[value->size()] = '\0';
        if (GuiTextBox((Rectangle){pos.x, pos.y + 20, 420, 20}, name, 16, activeProp == (void*)value)) {activeProp = (void*)value;};
        *value = name;
    }
    void Engine::DisplayPositionSelector(Vector2* position, std::string propName, Vector2 pos) {
        if(GuiButton((Rectangle) {pos.x, pos.y, 100, 20}, propName.c_str())) {
            positionSelector = position;
        }
    }

    void Engine::DisplaySelectedFixtures(std::vector<Fixture*>* affectedFixtures, Vector2 p) {
         for(unsigned int i = 0; i < fixtures.size(); i++) {
            auto position = std::find(affectedFixtures->begin(), affectedFixtures->end(), fixtures[i]);
            bool actived = position != affectedFixtures->end();
            bool nextActived = GuiToggle((Rectangle) {(float)(p.x + (i % 4) * 105), p.y + (float)(i/4) * 20, 105, 20}, fixtures[i]->name_.c_str(), actived);
            bool clicked = actived != nextActived;
            if(clicked) {
                if(!nextActived)
                    affectedFixtures->erase(position);
                else
                    affectedFixtures->push_back(fixtures[i]);
            }
        }
        
    }

    void Engine::SelectPosition() {
        *positionSelector = GetMousePosition();
        positionSelector = nullptr;
    }
    void Engine::MouseLeftClick() {
        if (GetMousePosition().x > WIDTH - PANELSIZE) return;
        if (positionSelector != nullptr) {
            SelectPosition();
            return;
        }
        activeProp = nullptr;
        DeselectModifiable();
        for(unsigned int i = 0; i < fixtures.size(); i++) {
            if(CheckCollisionPointCircle(GetMousePosition(), fixtures[i]->position_, 10)) {
                selectedModifiable = fixtures[i];
                break;
            }
        }
        for(unsigned int i = 0; i < GetModifiers()->size(); i++) {
            if(CheckCollisionPointRec(GetMousePosition(), GetModifiers()->at(i)->GetSelector())) {
                selectedModifiable = GetModifiers()->at(i);
                break;
            }
        }
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
            fixturesArray.PushBack(fixtures[i]->Serialize(allocator), allocator);
        d.AddMember("fixtures", fixturesArray, allocator);

        rapidjson::Value allTheModifiers;
        allTheModifiers.SetArray();
        for(unsigned int i = 0; i < PAGES; i++) {
            rapidjson::Value modifiersPage;
            modifiersPage.SetArray();
            for(unsigned int j = 0; j < modifiers[i].size(); j++)
                modifiersPage.PushBack(modifiers[i][j]->Serialize(allocator), allocator);
            allTheModifiers.PushBack(modifiersPage, allocator);
        }
        d.AddMember("modifiers", allTheModifiers, allocator);

        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
        std::ofstream file("data.json");
        d.Accept(writer);
        file << sb.GetString();
    }

    void Engine::Load() {
        for(unsigned int i = 0; i < GetModifiers()->size(); i++)
            delete GetModifiers()->at(i);
        GetModifiers()->clear();
        for(unsigned int i = 0; i < fixtures.size(); i++)
            delete fixtures[i];
        fixtures.clear();

        std::ifstream file("data.json");
        std::stringstream ss;
        ss << file.rdbuf();
        rapidjson::Document d;
        d.Parse(ss.str().c_str());

        auto fixturesArray = d["fixtures"].GetArray();
        for(unsigned int i = 0; i < fixturesArray.Size(); i++)
            fixtures.push_back(new Fixture(this, fixturesArray[i]));

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
                modifiers->push_back(modif);
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

        InitWindow(1366, 768, "MSQ LIGHTS");
        SetExitKey(0);

        SetTargetFPS(60);

        GuiLoadStyleDark();
    }

    void Engine::Update() {
        for (unsigned int i = 0; i < DMX_ADDRESSES; i++) 
            dmxValues[i] = 0;
 
        for(unsigned int i = 0; i < fixtures.size(); i++)
            fixtures[i]->Update();

        if (follow.enabled_)
            for(unsigned int i = 0; i < follow.affectedFixtures_.size(); i++)
                follow.affectedFixtures_[i]->Blend(&follow);
 
        for(unsigned int i = 0; i < GetModifiers()->size(); i++) {
            for(unsigned int j = 0; j < GetModifiers()->at(i)->affectedFixtures_.size(); j++) {
                GetModifiers()->at(i)->affectedFixtures_[j]->Blend(GetModifiers()->at(i));
            }
            GetModifiers()->at(i)->Update();
        }
        if(IsKeyPressed(KEY_ENTER))
            activeProp = nullptr;
        if(activeProp == nullptr) {
            if(IsKeyPressed(KEY_S))
                Save();
            if(IsKeyPressed(KEY_L))
                Load();
            if(IsKeyPressed(KEY_F))
                follow.enabled_ = !follow.enabled_;
            if(IsKeyPressed(KEY_TAB))
                debug = !debug;
        }
        if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
            MouseLeftClick();
        if(follow.enabled_)
            follow.position_ = GetMousePosition();
        for(unsigned int i = 0; i < fixtures.size(); i++)
            fixtures[i]->WriteDmx();
        SendDmx();
        oscServer.Update();
        follow.Update();
    }

    void Engine::Draw() {
        ClearBackground((Color) {10,10,10,255});

        if(debug) {
            for(unsigned int i = 0; i < 32; i++)
                for(unsigned int j = 0; j < 16; j++)
                    GuiLabel((Rectangle){(float)j * 40, (float)i * 20, 40, 20}, TextFormat("%d", dmxValues[i* 16 + j]));
        }

        for(unsigned int i = 0; i < fixtures.size(); i++) {
            fixtures[i]->Draw();
        }
        for(unsigned int i = 0; i < GetModifiers()->size(); i++) {
            GetModifiers()->at(i)->Draw();
        }

        follow.Draw();

        GuiPanel((Rectangle) {946, 0, 420, 768}, "Properties");
        if(positionSelector != nullptr) {
            GuiLabel((Rectangle) {946, 500, 420, 20}, "Choose a position");
        }
        else {
            selectedModifiable->DrawProps();
            if(GuiButton((Rectangle){946, 748, 420, 20}, "Deselect"))
                DeselectModifiable();
        }
    }
    void Engine::Close() {
        oscServer.Close();
    }

    void Engine::Loop() {
        Init();
        while(!WindowShouldClose()) {
            Update();
            BeginDrawing();
            Draw();
            EndDrawing();
        }
        Close();
    }
}