#include "blueprint/StageFuncNode.h"
#include "blueprint/Node.h"
#include "blueprint/CompNode.h"
#include "blueprint/NodeHelper.h"
#include "blueprint/node/Function.h"

#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>

namespace bp
{

void StageFuncNode::SetFilepath(const std::string& filepath)
{
    if (m_parent_node) {
        if (m_parent_node->HasUniqueComp<CompNode>()) {
            auto& cnode = m_parent_node->GetUniqueComp<CompNode>();
            auto bp_node = cnode.GetNode();
            if (bp_node->get_type() == rttr::type::get<node::Function>()) {
                auto func_node = std::static_pointer_cast<node::Function>(bp_node);
                func_node->SetFilepath(filepath);
            }
        }
    }
}

void StageFuncNode::UpdateParentAABB(const NodePtr& node)
{
    auto& st = node->GetStyle();
    m_parent_node->GetUniqueComp<n2::CompBoundingBox>().SetSize(
        *m_parent_node, sm::rect(st.width, st.height)
    );
}

void StageFuncNode::InsertSceneObj(const ur::Device& dev, const n0::SceneNodePtr& node)
{
    // load function nodes
    auto& cnode = node->GetUniqueComp<bp::CompNode>();
    auto bp_node = cnode.GetNode();
    if (bp_node->get_type() == rttr::type::get<bp::node::Function>()) {
        NodeHelper::LoadFunctionNode(dev, node, bp_node);
    }

    if (m_insert_enable && m_parent_node)
    {
        if (m_parent_node->HasUniqueComp<bp::CompNode>())
        {
            auto& cnode = m_parent_node->GetUniqueComp<bp::CompNode>();
            auto bp_node = cnode.GetNode();
            if (bp_node->get_type() == rttr::type::get<bp::node::Function>()) {
                auto func_node = std::static_pointer_cast<bp::node::Function>(bp_node);
                if (bp::node::Function::AddChild(func_node, node)) {
                    UpdateParentAABB(func_node);
                }
            }
        }
    }
}

void StageFuncNode::DeleteSceneObj(const n0::SceneNodePtr& node)
{
    if (m_parent_node)
    {
        if (m_parent_node->HasUniqueComp<bp::CompNode>())
        {
            auto& cnode = m_parent_node->GetUniqueComp<bp::CompNode>();
            auto bp_node = cnode.GetNode();
            if (bp_node->get_type() == rttr::type::get<bp::node::Function>()) {
                auto func_node = std::static_pointer_cast<bp::node::Function>(bp_node);
                if (bp::node::Function::RemoveChild(func_node, node)) {
                    UpdateParentAABB(func_node);
                }
            }
        }
    }
}

void StageFuncNode::ClearSceneObj()
{
    if (m_parent_node)
    {
        if (m_parent_node->HasUniqueComp<bp::CompNode>())
        {
            auto& cnode = m_parent_node->GetUniqueComp<bp::CompNode>();
            auto bp_node = cnode.GetNode();
            if (bp_node->get_type() == rttr::type::get<bp::node::Function>()) {
                auto func_node = std::static_pointer_cast<bp::node::Function>(bp_node);
                func_node->ClearChildren();
                UpdateParentAABB(func_node);
            }
        }
    }
}

}