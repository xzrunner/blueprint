#pragma once

#include "blueprint/CompNode.h"
#include "blueprint/MessageID.h"
#include "blueprint/CommentaryNodeHelper.h"
#include "blueprint/node/SubGraph.h"

#include <ee0/MsgHelper.h>

#include <ns/CompFactory.h>
#include <ns/CompSerializer.h>
#include <node0/NodeFlags.h>
#include <node0/NodeFlagsHelper.h>
#include <node2/AABBSystem.h>
#include <node2/CompBoundingBox.h>

namespace bp
{

template<typename T>
void Serializer<T>::LoadFromJson(const ur::Device& dev, ee0::WxStagePage& stage, const n0::SceneNodePtr& root,
                                 const rapidjson::Value& val, const std::string& dir)
{
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

    // update subgraph
    assert(casset->TypeID() == n0::GetAssetUniqueTypeID<n0::CompComplex>());
    auto ccomplex = std::static_pointer_cast<n0::CompComplex>(casset);
    for (auto& c : ccomplex->GetAllChildren())
    {
        if (!c->HasUniqueComp<CompNode>()) {
            continue;
        }
        auto bp_node = c->GetUniqueComp<CompNode>().GetNode();
        if (!bp_node->get_type().is_derived_from<node::SubGraph<T>>()) {
            continue;
        }
        auto& c_ccomplex = c->HasSharedComp<n0::CompComplex>() ? 
            c->GetSharedComp<n0::CompComplex>() : c->AddSharedComp<n0::CompComplex>();
        std::vector<bp::NodePtr> bp_nodes;
        for (auto& cc : c_ccomplex.GetAllChildren())
        {
            n0::NodeFlagsHelper::SetFlag<n0::NodeNotVisible>(*cc, true);
            if (cc->HasUniqueComp<CompNode>()) {
                bp_nodes.push_back(cc->GetUniqueComp<CompNode>().GetNode());
            }
        }
        auto sub_graph = std::static_pointer_cast<node::SubGraph<T>>(bp_node);
        sub_graph->SetChildren(bp_nodes, true);
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

template<typename T>
void Serializer<T>::StoreToJson(const n0::SceneNodePtr& root, const std::string& dir,
                                rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc)
{
    assert(root->HasSharedComp<n0::CompComplex>());
    ns::CompSerializer::Instance()->ToJson(
        root->GetSharedComp<n0::CompComplex>(), dir, val, alloc, false
    );
}

}