#include "Engine.hpp"
#include <cstring>
#include "dark.h"
#include <cstring>

namespace MsqLights {
    Engine::EmptyModifiable::EmptyModifiable(Engine* engine)
    : Modifiable(engine) {
        selectedFixture_ = 0;
        selectedModifier_ = 0;
    }

    void Engine::EmptyModifiable::DrawProps() {
        if(GuiButton((Rectangle) {1500, 20, 100, 20}, "Spawn Fixture")){
            engine_->fixtures.push_back(new Fixture(engine_, (Vector2){500, 500}, (Vector2){500, 500}, "Fixture", 0, Fixture::Mode::Dimmer));
            engine_->selectedModifiable = engine_->fixtures[engine_->fixtures.size() - 1];
        }
        if(GuiButton((Rectangle) {1600, 20, 100, 20}, "Spawn Rectangle")){
            RectangleModifier* r = new RectangleModifier(engine_);
            engine_->modifiers.push_back(r);
            r->name_ = "Rectangle";
            engine_->selectedModifiable = r;
            
        }
        if(GuiButton((Rectangle) {1700, 20, 100, 20}, "Spawn Spot")){
            SpotModifier* s = new SpotModifier(engine_);
            engine_->modifiers.push_back(s);
            s->name_ = "Spot";
            engine_->selectedModifiable = s;
        }
        
        std::string fixturesNames;
        for(unsigned int i = 0; i < engine_->fixtures.size(); i++) {
            fixturesNames += engine_->fixtures[i]->name_;
            fixturesNames += ";";
        }
        fixturesNames.erase(fixturesNames.end() - 1);

        if (!engine_->fixtures.empty()) {
            if (GuiButton((Rectangle){1800, 40, 60, 20}, "Select"))
                engine_->selectedModifiable = engine_->fixtures[selectedFixture_];
            if (GuiButton((Rectangle){1860, 40, 60, 20}, "Delete"))
                engine_->fixtures.erase(engine_->fixtures.begin() + selectedFixture_); 
        }
        std::string modifiersNames;
        for(unsigned int i = 0; i < engine_->modifiers.size(); i++) {
            modifiersNames += engine_->modifiers[i]->name_;
            modifiersNames += ";";
        }
        modifiersNames.erase(modifiersNames.end() - 1);

        if (!engine_->modifiers.empty()) {
            if (GuiButton((Rectangle){1800, 60, 60, 20}, "Select"))
                engine_->selectedModifiable = engine_->modifiers[selectedModifier_];
            if (GuiButton((Rectangle){1860, 60, 60, 20}, "Delete")) {
                auto m = engine_->modifiers.begin() + selectedModifier_;
                delete (*m);
                engine_->modifiers.erase(m); 
            }
        }
        
        engine_->follow.DrawProps();

        if (GuiDropdownBox((Rectangle) {1500, 60, 300, 20}, modifiersNames.c_str(), &selectedModifier_, engine_->activeProp == &selectedModifier_)) {
            if (engine_->activeProp == &selectedModifier_)
                engine_->activeProp = nullptr;
            else
                engine_->activeProp = &selectedModifier_;
        }

        if (GuiDropdownBox((Rectangle) {1500, 40, 300, 20}, fixturesNames.c_str(), &selectedFixture_, engine_->activeProp == &selectedFixture_)) {
            if (engine_->activeProp == &selectedFixture_)
                engine_->activeProp = nullptr;
            else
                engine_->activeProp = &selectedFixture_;
        }
    }

    Engine::Engine() 
    : follow(this), emptyModifiable(this) {
        selectedModifiable = &emptyModifiable;
        activeProp = nullptr;
        debug = false;
        positionSelector = nullptr;
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
    void Engine::SelectPosition() {
        *positionSelector = GetMousePosition();
        positionSelector = nullptr;
    }
    void Engine::MouseLeftClick() {
        if (GetMousePosition().x > 1500) return;
        if (positionSelector != nullptr) {
            SelectPosition();
            return;
        }
        activeProp = nullptr;
        DeselectModifiable();
        unsigned int i = 0;
        for(unsigned int i = 0; i < fixtures.size(); i++) {
            if(CheckCollisionPointCircle(GetMousePosition(), fixtures[i]->position_, 10)) {
                selectedModifiable = fixtures[i];
                break;
            }
        }
        for(unsigned int i = 0; i < modifiers.size(); i++) {
            if(CheckCollisionPointRec(GetMousePosition(), modifiers[i]->GetSelector())) {
                selectedModifiable = modifiers[i];
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
        std::vector<char> data;
        for(unsigned int i = 0; i < fixtures.size(); i++) {
            std::vector<char> v = fixtures[i]->Serialize();
            data.insert(data.end(), v.begin(), v.end());
            data.push_back('\n');
        }

        SaveFileData("data", &data[0], data.size());
    }

    void Engine::Load() {
        for(Fixture* f: fixtures)
            delete f;
        fixtures.clear();
        unsigned int dataSize = 0;
        unsigned char* data = LoadFileData("data", &dataSize);
        unsigned char* ptrInFile = data;
        while (ptrInFile - data < dataSize) {
            Fixture* f = new Fixture(this, ptrInFile);
            fixtures.push_back(f);
            ptrInFile += sizeof(Fixture);
            ptrInFile += std::strlen((char*)ptrInFile);
            ptrInFile += 2;
        }
    }

    void Engine::Init() {
        for (unsigned int i = 0; i < DMX_ADDRESSES; i++) 
            dmxValues[i] = 0;
        dmxClient.Setup();
        SendDmx();
    
        DeselectModifiable();

        InitWindow(1920, 1080, "raylib");

        SetTargetFPS(60);

        GuiLoadStyleDark();
    }

    void Engine::Update() {
        for (unsigned int i = 0; i < DMX_ADDRESSES; i++) 
            dmxValues[i] = 0;
 
        for(unsigned int i = 0; i < fixtures.size(); i++)
            fixtures[i]->color_ = BLACK;

        for(unsigned int i = 0; i < follow.affectedFixtures_.size(); i++)
            fixtures[i]->Blend(&follow);
 
        for(unsigned int i = 0; i < modifiers.size(); i++) {
            for(unsigned int j = 0; j < modifiers[i]->affectedFixtures_.size(); j++) {
                modifiers[i]->affectedFixtures_[j]->Blend(modifiers[i]);
            }
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
        for(unsigned int i = 0; i < modifiers.size(); i++) {
            modifiers[i]->Draw();
        }

        follow.Draw();

        GuiPanel((Rectangle) {1500, 0, 420, 1080}, "Properties");
        if(positionSelector != nullptr) {
            GuiLabel((Rectangle) {1500, 500, 420, 20}, "Choose a position");
        }
        else {
            selectedModifiable->DrawProps();
            if(GuiButton((Rectangle){1500, 1060, 420, 20}, "Deselect"))
                DeselectModifiable();
        }
    }
    void Engine::Close() {

    }

    void Engine::Loop() {
        Init();
        while(!WindowShouldClose()) {
            Update();
            BeginDrawing();
            Draw();
            EndDrawing();
        }
    }
}