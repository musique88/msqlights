#include "Modifiable.hpp"
#include "Engine.hpp"
#include <utility>

namespace MsqLights {
    Modifiable::Modifiable(Engine* engine) {
       engine_ = engine; 
       id_ = engine_->random();
    }

    void Modifiable::DrawProps() {
        DrawText(TextFormat("%#x", id_), GetScreenWidth() - 200, 0, 20, WHITE);
    }

    rapidjson::Value Modifiable::Serialize(rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value val;
        val.SetObject();

        for (auto p : parameters_){
            rapidjson::Value paramName;
            paramName.SetString(p.first.c_str(), allocator);
            rapidjson::Value value;

            switch(p.second.type_) {
                case Parameter::Type::FLOAT:
                    value.SetFloat(p.second.getFloat());
                    break;
                case Parameter::Type::INT:
                    value.SetInt(p.second.getInt());
                    break;
                case Parameter::Type::UINT:
                    value.SetUint(p.second.getUInt());
                    break;
                case Parameter::Type::UCHAR:
                    value.SetUint(p.second.getUChar());
                    break;
                case Parameter::Type::STRING:
                    value.SetString(p.second.getString().c_str(), allocator);
                    break;
                default:
                    break;
            }
            val.AddMember(paramName, value, allocator);
        }
        return val;
    }

    void Modifiable::RegisterParameters() {
        parameters_.emplace(std::make_pair(std::string("id"), Parameter(&id_, true)));
    }

    void Modifiable::Init() {
        RegisterParameters();
    }

    void Modifiable::Load(rapidjson::Value& val) {
        for (auto p : parameters_) {
            switch(p.second.type_) {
                case Parameter::Type::FLOAT:
                    p.second.setVal(val[p.first.c_str()].GetFloat());
                    break;
                case Parameter::Type::INT:
                    p.second.setVal(val[p.first.c_str()].GetInt());
                    break;
                case Parameter::Type::UINT:
                    p.second.setVal(val[p.first.c_str()].GetUint());
                    break;
                case Parameter::Type::UCHAR:
                    p.second.setVal(val[p.first.c_str()].GetUint());
                    break;
                case Parameter::Type::STRING:
                    p.second.setVal(val[p.first.c_str()].GetString());
                    break;
            }
        }
    }
}