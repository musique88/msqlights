#pragma once

#include <raylib.h>
#include <rapidjson/document.h>

namespace MsqLights {
    rapidjson::Value Serialize(Color c, rapidjson::Document::AllocatorType& allocator);
    rapidjson::Value Serialize(Vector2 v, rapidjson::Document::AllocatorType& allocator);
    rapidjson::Value Serialize(Rectangle r, rapidjson::Document::AllocatorType& allocator);
    Color ColorParse(rapidjson::Value& val);
    Vector2 Vector2Parse(rapidjson::Value& val);
    Rectangle RectangleParse(rapidjson::Value& val);

}
