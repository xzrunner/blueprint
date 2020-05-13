#include "blueprint/WxToolbarPanel.h"
#include "blueprint/WxNodeProperty.h"
#include "blueprint/CompNode.h"

#include <ee0/SubjectMgr.h>
#include <ee0/GameObj.h>

#include <node0/SceneNode.h>

#include <wx/sizer.h>

namespace bp
{

WxToolbarPanel::WxToolbarPanel(const ur::Device& dev, wxWindow* parent,
                               const ee0::SubjectMgrPtr& graph_sub_mgr)
    : wxPanel(parent)
    , m_graph_sub_mgr(graph_sub_mgr)
{
    InitLayout(dev);

    graph_sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
    graph_sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
}

WxToolbarPanel::~WxToolbarPanel()
{
    m_graph_sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
    m_graph_sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
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
	}
}

void WxToolbarPanel::SetDefaultProp(wxPanel* default_prop)
{
    m_default_prop = default_prop;

    auto sizer = GetSizer();
    const int num = sizer->GetItemCount();
    if (num > 1) {
        assert(num == 2);
        sizer->Remove(1);
    }
    sizer->Add(default_prop);

    default_prop->Hide();

    Layout();
}

void WxToolbarPanel::InitLayout(const ur::Device& dev)
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(m_node_prop = new WxNodeProperty(dev, this, m_graph_sub_mgr), wxEXPAND);

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

}