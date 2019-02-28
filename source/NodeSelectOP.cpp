#include "blueprint/NodeSelectOP.h"
#include "blueprint/CompNode.h"
#include "blueprint/node/SetLocalVar.h"
#include "blueprint/node/GetLocalVar.h"

#include <ee0/WxStagePage.h>

#include <node0/SceneNode.h>

namespace bp
{

NodeSelectOP::NodeSelectOP(const std::shared_ptr<pt0::Camera>& camera, ee0::WxStagePage& stage)
    : ee2::NodeSelectOP(camera, ECS_WORLD_VAR stage)
{
}

void NodeSelectOP::AfterInsertSelected(const n0::SceneNodePtr& node) const
{
    if (!node->HasUniqueComp<CompNode>()) {
        return;
    }

    auto bp_node = node->GetUniqueComp<CompNode>().GetNode();
    if (bp_node->get_type() == rttr::type::get<node::SetLocalVar>())
    {
        auto& name = std::static_pointer_cast<node::SetLocalVar>(bp_node)->GetVarName();
        ChangeVarHighlight(false, name, true);
    }
    else if (bp_node->get_type() == rttr::type::get<node::GetLocalVar>())
    {
        auto& name = std::static_pointer_cast<node::GetLocalVar>(bp_node)->GetVarName();
        ChangeVarHighlight(true, name, true);
    }
}

void NodeSelectOP::AfterDeleteSelected(const n0::SceneNodePtr& node) const
{
    if (!node->HasUniqueComp<CompNode>()) {
        return;
    }


    auto bp_node = node->GetUniqueComp<CompNode>().GetNode();
    if (bp_node->get_type() == rttr::type::get<node::SetLocalVar>())
    {
        auto& name = std::static_pointer_cast<node::SetLocalVar>(bp_node)->GetVarName();
        ChangeVarHighlight(false, name, false);
    }
    else if (bp_node->get_type() == rttr::type::get<node::GetLocalVar>())
    {
        auto& name = std::static_pointer_cast<node::GetLocalVar>(bp_node)->GetVarName();
        ChangeVarHighlight(true, name, false);
    }
}

void NodeSelectOP::AfterClearSelection() const
{
    ClearVarHighlight();
}

void NodeSelectOP::ChangeVarHighlight(bool is_set_var, const std::string& name, bool set_highlight) const
{
    if (name.empty()) {
        return;
    }

    m_stage.Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (!obj->HasUniqueComp<CompNode>()) {
            return true;
        }
        auto& cnode = obj->GetUniqueComp<CompNode>();
        auto node = cnode.GetNode();
        auto type = node->get_type();
        if (is_set_var && type == rttr::type::get<node::SetLocalVar>() &&
            std::static_pointer_cast<node::SetLocalVar>(node)->GetVarName() == name) {
            SetNodeBGColor(node, set_highlight ? BG_COLOR_HIGHLIGHT : BG_COLOR_DEFAULT);
        } else if (!is_set_var && type == rttr::type::get<node::GetLocalVar>() &&
            std::static_pointer_cast<node::GetLocalVar>(node)->GetVarName() == name) {
            SetNodeBGColor(node, set_highlight ? BG_COLOR_HIGHLIGHT : BG_COLOR_DEFAULT);
        }
        return true;
    });
}

void NodeSelectOP::ClearVarHighlight() const
{
    m_stage.Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (!obj->HasUniqueComp<CompNode>()) {
            return true;
        }
        auto& cnode = obj->GetUniqueComp<CompNode>();
        auto node = cnode.GetNode();
        auto type = node->get_type();
        if (type == rttr::type::get<node::SetLocalVar>() ||
            type == rttr::type::get<node::GetLocalVar>()) {
            SetNodeBGColor(node, BG_COLOR_DEFAULT);
        }
        return true;
    });
}

void NodeSelectOP::SetNodeBGColor(const NodePtr& node, const pt0::Color& color)
{
    auto st = node->GetStyle();
    st.panel_bg_col = color;
    node->SetStyle(st);
}

}