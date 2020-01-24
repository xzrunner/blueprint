#pragma once

#include <node0/typedef.h>

#include <rapidjson/document.h>

namespace ee0 { class WxStagePage; }

namespace bp
{

class Serializer
{
public:
    static void LoadFromJson(ee0::WxStagePage& stage, const n0::SceneNodePtr& root,
        const rapidjson::Value& val, const std::string& dir);
    static void StoreToJson(const n0::SceneNodePtr& root, const std::string& dir,
        rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc);

private:
    static void SetupConnCB();

}; // Serializer

}