#pragma once

#include "Interfaces.hpp"
#include <rapidjson/document.h>
#include <map>
#include <variant>

namespace MsqLights {
    class Modifiable : public Serializable {
    protected:
        Engine* engine_;
        std::map<std::string, std::variant<
            float*, 
            int*, 
            unsigned int*, 
            unsigned char*, 
            std::string*>
        > parameters_;
        unsigned int id_;
    public:
        enum Type {
            None,
            GlobalModifier,
            SpotModifier,
            Fixture
        } type_;

        Modifiable(Engine* e);
        virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;
        virtual void RegisterParameters();
        virtual void Load(rapidjson::Value& val);
        void Init();
        virtual void Update();
    };
}