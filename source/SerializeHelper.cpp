#include "blueprint/SerializeHelper.h"
#include "blueprint/NSCompNode.h"

#include <memmgr/LinearAllocator.h>
#include <ns/CompSerializer.h>
#include <ns/N0CompComplex.h>
#include <node0/CompComplex.h>

namespace bp
{

void SerializeHelper::SetupConnCB()
{
    ns::CompSerializer::Instance()->AddFromJsonFunc(n0::CompComplex::TYPE_NAME,
        [](const ur::Device& dev, n0::NodeComp& comp, const std::string& dir, const rapidjson::Value& val)
    {
        auto& ccomplex = static_cast<n0::CompComplex&>(comp);

        ns::N0CompComplex seri;
        mm::LinearAllocator alloc;
        seri.LoadFromJson(dev, alloc, dir, val);
        seri.StoreToMem(dev, ccomplex);

        NSCompNode::LoadConnection(ccomplex.GetAllChildren(), val["nodes"]);
    }, true);

    ns::CompSerializer::Instance()->AddToJsonFunc(n0::CompComplex::TYPE_NAME,
        [](const n0::NodeComp& comp, const std::string& dir, rapidjson::Value& val,
            rapidjson::MemoryPoolAllocator<>& alloc, bool skip_asset)->bool
    {
        auto& ccomplex = static_cast<const n0::CompComplex&>(comp);

        ns::N0CompComplex seri;
        seri.LoadFromMem(ccomplex);
        seri.StoreToJson(dir, val, alloc);

        NSCompNode::StoreConnection(ccomplex.GetAllChildren(), val["nodes"], alloc);

        return true;
    }, true);
}


}