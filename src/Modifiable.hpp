#pragma once

#include "Interfaces.hpp"
#include "Parameter.hpp"
#include <rapidjson/document.h>
#include <map>

namespace MsqLights {
    class Modifiable : public Serializable {
    protected:
        Engine* engine_;
        std::map<std::string, Parameter> parameters_;
        unsigned int id_;
    public:
        virtual void DrawProps();
        Modifiable(Engine* e);
        virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;
        virtual void RegisterParameters();
        virtual void Load(rapidjson::Value& val);
        void Init();
    };
}