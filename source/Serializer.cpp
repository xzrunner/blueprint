#include "blueprint/Serializer.h"
#include "blueprint/CommentaryNodeHelper.h"
#include "blueprint/NSCompNode.h"
#include "blueprint/MessageID.h"
#include "blueprint/CompNode.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>
#include <ee0/MsgHelper.h>

#include <memmgr/LinearAllocator.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/AABBSystem.h>
#include <node2/CompBoundingBox.h>
#include <ns/CompFactory.h>
#include <ns/CompSerializer.h>
#include <ns/N0CompComplex.h>
#include <sx/ResFileHelper.h>
#include <js/RapidJsonHelper.h>

#include <boost/filesystem.hpp>

namespace bp
{

void Serializer::LoadFromJson(const ur2::Device& dev, ee0::WxStagePage& stage, const n0::SceneNodePtr& root,
                              const rapidjson::Value& val, const std::string& dir)
{
    SetupConnCB();

    auto sub_mgr = stage.GetSubjectMgr();

    sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
    sub_mgr->NotifyObservers(ee0::MSG_SCENE_NODE_CLEAR);

    n0::CompAssetPtr casset = ns::CompFactory::Instance()->CreateAsset(dev, val, dir);
    if (casset)
    {
        if (root->HasSharedComp<n0::CompAsset>()) {
            root->RemoveSharedComp<n0::CompAsset>();
        }
        root->AddSharedCompNoCreate<n0::CompAsset>(casset);
    }
    else
    {
        casset = root->GetSharedCompPtr<n0::CompAsset>();
    }

    // FIXME: reinsert, for send insert msg to other panel
    if (root->HasSharedComp<n0::CompComplex>())
    {
        auto& ccomplex = root->GetSharedComp<n0::CompComplex>();
        auto children = ccomplex.GetAllChildren();
        ccomplex.RemoveAllChildren();
        sub_mgr->NotifyObservers(MSG_BP_DISABLE_UPDATE_EVAL);
        for (auto& c : children) {
            ee0::MsgHelper::InsertNode(*sub_mgr, c, false);
        }
        sub_mgr->NotifyObservers(MSG_BP_ENABLE_UPDATE_EVAL);
    }

    // connection
    CommentaryNodeHelper::InsertNodeToCommentary(stage);
    sub_mgr->NotifyObservers(MSG_BP_CONN_REBUILD);

    // aabb
	if (root->HasUniqueComp<n2::CompBoundingBox>())
	{
		auto& cbb = root->GetUniqueComp<n2::CompBoundingBox>();
		auto aabb = n2::AABBSystem::Instance()->GetBounding(*root);
		cbb.SetSize(*root, aabb);
	}

	sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void Serializer::StoreToJson(const n0::SceneNodePtr& root, const std::string& dir,
                             rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc)
{
    SetupConnCB();

    assert(root->HasSharedComp<n0::CompComplex>());
    ns::CompSerializer::Instance()->ToJson(
        root->GetSharedComp<n0::CompComplex>(), dir, val, alloc, false
    );
}

void Serializer::SetupConnCB()
{
    ns::CompSerializer::Instance()->AddFromJsonFunc(n0::CompComplex::TYPE_NAME,
        [](const ur2::Device& dev, n0::NodeComp& comp, const std::string& dir, const rapidjson::Value& val)
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