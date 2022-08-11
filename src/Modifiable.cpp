#include "Modifiable.hpp"
#include "Engine.hpp"
#include <utility>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace MsqLights {
    Modifiable::Modifiable(Engine* engine) {
       engine_ = engine; 
       id_ = engine_->random();
       type_ = Type::None;
    }

    rapidjson::Value Modifiable::Serialize(rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value val;
        val.SetObject();

        for (auto p : parameters_){
            rapidjson::Value paramName;
            paramName.SetString(p.first.c_str(), allocator);
            rapidjson::Value param;
            std::visit(overloaded {
                [&](float* arg) { param.SetFloat(*arg); },
                [&](int* arg) { param.SetInt(*arg); },
                [&](unsigned int* arg) { param.SetUint(*arg); },
                [&](unsigned char* arg) { param.SetUint(*arg); },
                [&](std::string* arg) { param.SetString(arg->c_str(), allocator); },
                },
                p.second
            );
            val.AddMember(paramName, param, allocator);
        }
        return val;
    }

    void Modifiable::RegisterParameters() {
        parameters_.emplace(std::string("id"), &id_);
        parameters_.emplace(std::string("type"), (unsigned int*)&type_);
    }

    void Modifiable::Init() {
        RegisterParameters();
    }

    void Modifiable::Load(rapidjson::Value& val) {
        for (auto p : parameters_) {
            std::visit(overloaded {
                [&](float* arg) { *arg = val[p.first.c_str()].GetFloat(); },
                [&](int* arg) { *arg = val[p.first.c_str()].GetInt(); },
                [&](unsigned int* arg) { *arg = val[p.first.c_str()].GetUint(); },
                [&](unsigned char* arg) { *arg = val[p.first.c_str()].GetUint(); },
                [&](std::string* arg) { *arg = val[p.first.c_str()].GetString(); },
                },
                p.second
            );
        }
    }

    void Modifiable::Update() {
        
    }
}