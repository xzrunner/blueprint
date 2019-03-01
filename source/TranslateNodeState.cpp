#include "blueprint/TranslateNodeState.h"
#include "blueprint/CompNode.h"
#include "blueprint/CommentaryNodeHelper.h"
#include "blueprint/node/Commentary.h"

#include <ee0/WxStagePage.h>
#include <ee0/CameraHelper.h>
#include <ee0/MsgHelper.h>
#include <ee2/NodeReorderHelper.h>

#include <node0/SceneNode.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>

#include <map>

namespace bp
{

TranslateNodeState::TranslateNodeState(ee0::WxStagePage& stage,
                                       const std::shared_ptr<pt0::Camera>& camera)
    : ee2::TranslateNodeState(camera, stage.GetSubjectMgr(), stage.GetSelection())
    , m_stage(stage)
{
}

bool TranslateNodeState::OnMouseRelease(int x, int y)
{
    if (ee2::TranslateNodeState::OnMouseRelease(x, y)) {
        return true;
    }

    // translated
    if (!m_selected_aabbs.empty()) {
        UpdateSelectionCommentary();
        m_selected_aabbs.clear();
    }

    return false;
}

bool TranslateNodeState::Clear()
{
    if (ee2::TranslateNodeState::Clear()) {
        return true;
    }

    m_selected_aabbs.clear();

    return false;
}

void TranslateNodeState::Translate(const sm::vec2& offset)
{
    if (m_selection.IsEmpty()) {
        return;
    }

    if (m_selected_aabbs.empty()) {
        GetSelectedAABBs(m_selected_aabbs);
    }

    ee2::TranslateNodeState::Translate(offset);

    m_selection.Traverse([&](const ee0::GameObjWithPos& opw)->bool
    {
        auto& scene_node = opw.GetNode();
        assert(scene_node->HasUniqueComp<CompNode>());
        auto& cnode = scene_node->GetUniqueComp<CompNode>();
        auto bp_node = cnode.GetNode();
        if (bp_node->get_type() == rttr::type::get<node::Commentary>())
        {
            auto comm = std::static_pointer_cast<node::Commentary>(bp_node);
            Translate(comm->GetAllChildren(), offset);
        }
        return false;
    });
}

void TranslateNodeState::Translate(const std::vector<std::weak_ptr<Node>>& bp_nodes,
                                   const sm::vec2& offset) const
{
    if (bp_nodes.empty()) {
        return;
    }

    std::map<NodePtr, n0::SceneNodePtr> map_bp2scene;
    m_stage.Traverse([&map_bp2scene](const ee0::GameObj& obj)->bool
    {
        if (obj->HasUniqueComp<CompNode>()) {
            auto& cnode = obj->GetUniqueComp<CompNode>();
            map_bp2scene.insert({ cnode.GetNode(), obj });
        }
        return true;
    });

    for (auto& bp_node : bp_nodes)
    {
        auto node = bp_node.lock();
        if (!node) {
            continue;
        }

        auto itr = map_bp2scene.find(node);
        if (itr == map_bp2scene.end()) {
            continue;
        }

        auto& scene_node = itr->second;
        // already translated
        if (m_selection.IsExist(ee0::GameObjWithPos(scene_node, scene_node, 0))) {
            continue;
        }

        auto& ctrans = scene_node->GetUniqueComp<n2::CompTransform>();
        ctrans.SetPosition(*scene_node, ctrans.GetTrans().GetPosition() + offset);
    }
}

bool TranslateNodeState::UpdateSelectionCommentary() const
{
    bool dirty = false;

    std::vector<sm::rect> new_aabbs;
    GetSelectedAABBs(new_aabbs);
    // fixme
    if (new_aabbs.size() != m_selected_aabbs.size()) {
        return false;
    }
     
    int idx = 0;
    m_selection.Traverse([&](const ee0::GameObjWithPos& opw)->bool
    {
        auto& node = opw.GetNode();
        if (!node->HasUniqueComp<CompNode>()) {
            return true;
        }
        // commentary can't insert to another commentary
        auto& bp_node = node->GetUniqueComp<CompNode>().GetNode();
        if (bp_node->get_type() == rttr::type::get<node::Commentary>()) {
            return true;
        }

        if (UpdateNodeCommentary(node, m_selected_aabbs[idx], new_aabbs[idx])) {
            dirty = true;
        }
        ++idx;
        return true;
    });

    return dirty;
}

bool TranslateNodeState::UpdateNodeCommentary(const n0::SceneNodePtr& node,
                                              const sm::rect& src, const sm::rect& dst) const
{
    bool ret = false;

    auto src_nodes = CommentaryNodeHelper::QueryCommNodeByRect(m_stage, src);
    auto dst_nodes = CommentaryNodeHelper::QueryCommNodeByRect(m_stage, dst);

    // remove
    auto bp_node = node->GetUniqueComp<CompNode>().GetNode();
    for (auto& src : src_nodes)
    {
        auto src_bp_node = src->GetUniqueComp<CompNode>().GetNode();
        assert(src_bp_node->get_type() == rttr::type::get<node::Commentary>());
        auto comm_node = std::static_pointer_cast<node::Commentary>(src_bp_node);
        if (comm_node->RemoveChild(bp_node)) {
            ret = true;
            break;
        }
    }

    // insert
    if (dst_nodes.empty()) {
        return ret;
    }

    n0::SceneNodePtr dst_node = dst_nodes[0];
    assert(dst_node);

    ret = true;

    assert(node->HasUniqueComp<CompNode>());
    auto& cnode = node->GetUniqueComp<CompNode>();
    auto parent_node = dst_node->GetUniqueComp<CompNode>().GetNode();
    assert(parent_node->get_type() == rttr::type::get<node::Commentary>());
    std::static_pointer_cast<node::Commentary>(parent_node)->AddChild(cnode.GetNode());

    ee2::NodeReorderHelper::SortNodes(*m_sub_mgr, node, dst_node);

    return ret;
}

void TranslateNodeState::GetSelectedAABBs(std::vector<sm::rect>& aabbs) const
{
    aabbs.clear();

    aabbs.reserve(m_selection.Size());
    m_selection.Traverse([&](const ee0::GameObjWithPos& opw)->bool
    {
        auto& obj = opw.GetNode();
        auto& cbb = obj->GetUniqueComp<n2::CompBoundingBox>();
        aabbs.push_back(cbb.GetBounding(*obj).GetSize());
        return true;
    });
}

}