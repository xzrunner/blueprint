#include "blueprint/WxToolbarPanel.h"
#include "blueprint/WxNodeProperty.h"
#include "blueprint/CompNode.h"
#include "blueprint/MessageID.h"

#include <ee0/SubjectMgr.h>
#include <ee0/GameObj.h>
#include <ee0/WxNavigationBar.h>

#include <node0/SceneNode.h>

#include <wx/sizer.h>

namespace bp
{

WxToolbarPanel::WxToolbarPanel(const ur::Device& dev, wxWindow* parent,
                               const ee0::SubjectMgrPtr& graph_sub_mgr,
                               bool nav_bar)
    : wxPanel(parent)
    , m_graph_sub_mgr(graph_sub_mgr)
{
    InitLayout(dev, nav_bar);

    graph_sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
    graph_sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
    graph_sub_mgr->RegisterObserver(MSG_BP_SCENE_ROOT_TO_NEXT_LEVEL, this);
}

WxToolbarPanel::~WxToolbarPanel()
{
    m_graph_sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
    m_graph_sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
    m_graph_sub_mgr->UnregisterObserver(MSG_BP_SCENE_ROOT_TO_NEXT_LEVEL, this);
}

void WxToolbarPanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_NODE_SELECTION_INSERT:
		OnSelectionInsert(variants);
		break;
    case ee0::MSG_NODE_SELECTION_CLEAR:
        OnSelectionClear(variants);
        break;
    case MSG_BP_SCENE_ROOT_TO_NEXT_LEVEL:
        OnRootToNextLevel(variants);
        break;
	}
}

void WxToolbarPanel::SetDefaultProp(wxPanel* default_prop)
{
    m_default_prop = default_prop;

    auto sizer = GetSizer();
    sizer->Insert(0, default_prop, wxEXPAND);
    default_prop->Hide();

    Layout();
}

void WxToolbarPanel::InitLayout(const ur::Device& dev, bool nav_bar)
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    // property
    sizer->Add(m_node_prop = new WxNodeProperty(dev, this, m_graph_sub_mgr), wxEXPAND);

    // nav bar
    if (nav_bar)
    {
        m_nav_bar = new ee0::WxNavigationBar(this);
        m_nav_bar->Push("obj");
        m_nav_bar->SetSeekCallback([&](size_t depth)
        {
            ee0::VariantSet vars;

            ee0::Variant var;
            var.m_type = ee0::VT_ULONG;
            var.m_val.ul = depth;
            vars.SetVariant("depth", var);

            m_graph_sub_mgr->NotifyObservers(MSG_BP_SCENE_ROOT_SEEK_TO_PREV_LEVEL, vars);
        });
        sizer->Add(m_nav_bar);
    }

    SetSizer(sizer);
}

void WxToolbarPanel::OnSelectionInsert(const ee0::VariantSet& variants)
{
    if (m_default_prop) {
        m_default_prop->Hide();
    }
    m_node_prop->Show();
    Layout();

	auto var_obj = variants.GetVariant("obj");
	GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
	GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

	auto& cnode = obj->GetUniqueComp<CompNode>();
    m_node_prop->LoadFromNode(obj, cnode.GetNode());
}

void WxToolbarPanel::OnSelectionClear(const ee0::VariantSet& variants)
{
    if (m_default_prop) {
        m_default_prop->Show();
    }
    m_node_prop->Hide();
    Layout();

    m_node_prop->Clear();
}

void WxToolbarPanel::OnRootToNextLevel(const ee0::VariantSet& variants)
{
    if (!m_nav_bar) {
        return;
    }

    auto var_obj = variants.GetVariant("obj");
    GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
    GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

    if (!obj->HasUniqueComp<bp::CompNode>()) {
        return;
    }
    auto& cnode = obj->GetUniqueComp<bp::CompNode>();
    auto& bp_node = cnode.GetNode();
    m_nav_bar->Push(bp_node->GetName());
}

}