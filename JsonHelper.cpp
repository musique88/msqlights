#include "JsonHelper.hpp"
namespace MsqLights {
    rapidjson::Value Serialize(Color c, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value col;
        col.SetObject();
        col.AddMember("r", c.r, allocator);
        col.AddMember("g", c.g, allocator);
        col.AddMember("b", c.b, allocator);
        col.AddMember("a", c.a, allocator);
        return col;
    }

    rapidjson::Value Serialize(Vector2 v, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value col;
        col.SetObject();
        col.AddMember("x", v.x, allocator);
        col.AddMember("y", v.y, allocator);
        return col;
    }

    rapidjson::Value Serialize(Rectangle r, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value val;
        val.SetObject();
        val.AddMember("x", r.x, allocator);
        val.AddMember("y", r.y, allocator);
        val.AddMember("w", r.width, allocator);
        val.AddMember("h", r.height, allocator);
        return val;
    }

    Color ColorParse(rapidjson::Value& val) {
        return (Color){
            (unsigned char)val["r"].GetInt(), 
            (unsigned char)val["g"].GetInt(), 
            (unsigned char)val["b"].GetInt(), 
            (unsigned char)val["a"].GetInt()
        };
    }

    Vector2 Vector2Parse(rapidjson::Value& val) {
        return (Vector2){
            val["x"].GetFloat(), 
            val["y"].GetFloat()
        };
    }

    Rectangle RectangleParse(rapidjson::Value& val) {
        return (Rectangle){
            val["x"].GetFloat(), 
            val["y"].GetFloat(), 
            val["w"].GetFloat(), 
            val["h"].GetFloat()
        };
    }
}