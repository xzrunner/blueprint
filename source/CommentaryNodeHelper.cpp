#include "blueprint/CommentaryNodeHelper.h"
#include "blueprint/CompNode.h"
#include "blueprint/node/Commentary.h"

#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>

#include <ee0/WxStagePage.h>

namespace bp
{

void CommentaryNodeHelper::InsertNodeToCommentary(ee0::WxStagePage& stage)
{
    std::vector<n0::SceneNodePtr> comm_nodes;
    stage.Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (!obj->HasUniqueComp<CompNode>()) {
            return  true;
        }
        auto& cnode = obj->GetUniqueComp<CompNode>();
        if (cnode.GetNode()->get_type() == rttr::type::get<node::Commentary>()) {
            comm_nodes.push_back(obj);
        }
        return true;
    });

    if (comm_nodes.empty()) {
        return;
    }

    stage.Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (!obj->HasUniqueComp<CompNode>()) {
            return  true;
        }
        auto& cnode = obj->GetUniqueComp<CompNode>();
        if (cnode.GetNode()->get_type() != rttr::type::get<node::Commentary>()) {
            InsertNodeToCommentary(obj, comm_nodes);
        }
        return true;
    });
}

void CommentaryNodeHelper::InsertNodeToCommentary(ee0::WxStagePage& stage, const n0::SceneNodePtr& node)
{
    assert(node->HasUniqueComp<n2::CompBoundingBox>());
    auto& cbb = node->GetUniqueComp<n2::CompBoundingBox>();
    auto rect = cbb.GetBounding(*node).GetSize();
    auto dst_nodes = QueryCommNodeByRect(stage, rect);
    if (dst_nodes.empty()) {
        return;
    }

    n0::SceneNodePtr dst_node = dst_nodes[0];
    assert(dst_node);

    assert(node->HasUniqueComp<CompNode>());
    auto& cnode = node->GetUniqueComp<CompNode>();
    auto parent_node = dst_node->GetUniqueComp<CompNode>().GetNode();
    assert(parent_node->get_type() == rttr::type::get<node::Commentary>());
    std::static_pointer_cast<node::Commentary>(parent_node)->AddChild(cnode.GetNode());
}

void CommentaryNodeHelper::InsertNodeToCommentary(const n0::SceneNodePtr& node,
                                                  const std::vector<n0::SceneNodePtr>& comm_nodes)
{
    n0::SceneNodePtr dst_node = nullptr;

    assert(node->HasUniqueComp<n2::CompBoundingBox>());
    auto& cbb = node->GetUniqueComp<n2::CompBoundingBox>();
    auto rect = cbb.GetBounding(*node).GetSize();
    for (auto& parent : comm_nodes)
    {
        auto& cbb = parent->GetUniqueComp<n2::CompBoundingBox>();
        if (cbb.GetBounding(*parent).IsContain(rect)) {
            dst_node = parent;
            break;
        }
    }

    if (!dst_node) {
        return;
    }
    
    assert(node->HasUniqueComp<CompNode>());
    auto& cnode = node->GetUniqueComp<CompNode>();
    auto parent_node = dst_node->GetUniqueComp<CompNode>().GetNode();
    assert(parent_node->get_type() == rttr::type::get<node::Commentary>());
    std::static_pointer_cast<node::Commentary>(parent_node)->AddChild(cnode.GetNode());
}

std::vector<n0::SceneNodePtr>
CommentaryNodeHelper::QueryCommNodeByRect(ee0::WxStagePage& stage, const sm::rect& r)
{
    std::vector<n0::SceneNodePtr> ret;
    stage.Traverse([&](const ee0::GameObj& obj)->bool
    {
        auto& cbb = obj->GetUniqueComp<n2::CompBoundingBox>();
        if (!cbb.GetBounding(*obj).IsContain(r)) {
            return true;
        }

        assert(obj->HasUniqueComp<CompNode>());
        auto& cnode = obj->GetUniqueComp<CompNode>();
        auto bp_node = cnode.GetNode();
        if (bp_node->get_type() == rttr::type::get<node::Commentary>()) {
            ret.push_back(obj);
        }
        return true;
    });
    return ret;
}

}