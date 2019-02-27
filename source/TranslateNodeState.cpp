#include "blueprint/TranslateNodeState.h"
#include "blueprint/CompNode.h"
#include "blueprint/node/Commentary.h"

#include <ee0/WxStagePage.h>
#include <ee0/CameraHelper.h>

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

    auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
    AddSelectionToCommentary(m_first_pos, pos);

    return false;
}

void TranslateNodeState::Translate(const sm::vec2& offset)
{
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
        if (node) {
            auto itr = map_bp2scene.find(node);
            if (itr != map_bp2scene.end()) {
                auto& scene_node = itr->second;
                auto& ctrans = scene_node->GetUniqueComp<n2::CompTransform>();
                ctrans.SetPosition(*scene_node, ctrans.GetTrans().GetPosition() + offset);
            }
        }
    }
}

std::vector<n0::SceneNodePtr> TranslateNodeState::QueryCommNodeByPos(const sm::vec2& pos) const
{
    std::vector<n0::SceneNodePtr> ret;
    m_stage.Traverse([&](const ee0::GameObj& obj)->bool
    {
        auto& cbb = obj->GetUniqueComp<n2::CompBoundingBox>();
        if (!cbb.GetBounding(*obj).IsContain(pos)) {
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

void TranslateNodeState::AddSelectionToCommentary(const sm::vec2& from, const sm::vec2& to) const
{
    auto new_nodes = QueryCommNodeByPos(to);
    auto old_nodes = QueryCommNodeByPos(from);

    bool new_contain_old = true;
    for (auto& old : old_nodes)
    {
        bool contain = false;
        for (auto& n : new_nodes) {
            if (old == n) {
                contain = true;
                break;
            }
        }
        if (!contain) {
            new_contain_old = false;
            break;
        }
    }
    if (old_nodes.empty()) {
        new_contain_old = false;
    }

    if (!new_contain_old)
    {
        // remove
        m_selection.Traverse([&](const ee0::GameObjWithPos& opw)->bool
        {
            for (auto& old : old_nodes)
            {
                if (old == opw.GetNode()) {
                    continue;
                }

                auto old_bp_node = old->GetUniqueComp<CompNode>().GetNode();
                assert(old_bp_node->get_type() == rttr::type::get<node::Commentary>());
                auto parent_bp = std::static_pointer_cast<node::Commentary>(old_bp_node);
                auto child_bp = opw.GetNode()->GetUniqueComp<CompNode>().GetNode();
                if (parent_bp->RemoveChild(child_bp)) {
                    break;
                }
            }
            return true;
        });

        // insert
        if (!new_nodes.empty())
        {
            auto& new_node = new_nodes[0];
            m_selection.Traverse([&](const ee0::GameObjWithPos& opw)->bool
            {
                if (new_node == opw.GetNode()) {
                    return true;
                }
                auto& scene_node = opw.GetNode();
                assert(scene_node->HasUniqueComp<CompNode>());
                auto& cnode = scene_node->GetUniqueComp<CompNode>();
                auto parent_node = new_node->GetUniqueComp<CompNode>().GetNode();
                assert(parent_node->get_type() == rttr::type::get<node::Commentary>());
                std::static_pointer_cast<node::Commentary>(parent_node)->AddChild(cnode.GetNode());
                return true;
            });
        }
    }
}

}