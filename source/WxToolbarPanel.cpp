#include "blueprint/WxToolbarPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee0/GameObj.h>
#include <blueprint/WxNodeProperty.h>
#include <blueprint/CompNode.h>

#include <node0/SceneNode.h>

#include <wx/sizer.h>

namespace bp
{

WxToolbarPanel::WxToolbarPanel(const ur2::Device& dev, wxWindow* parent,
                               const ee0::SubjectMgrPtr& graph_sub_mgr)
    : wxPanel(parent)
    , m_graph_sub_mgr(graph_sub_mgr)
{
    InitLayout(dev, graph_sub_mgr);

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

void WxToolbarPanel::InitLayout(const ur2::Device& dev, const ee0::SubjectMgrPtr& graph_sub_mgr)
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(m_node_prop = new WxNodeProperty(dev, this, graph_sub_mgr), wxEXPAND);

    SetSizer(sizer);
}

void WxToolbarPanel::OnSelectionInsert(const ee0::VariantSet& variants)
{
	auto var_obj = variants.GetVariant("obj");
	GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
	GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

	auto& cnode = obj->GetUniqueComp<CompNode>();
    m_node_prop->LoadFromNode(obj, cnode.GetNode());
}

void WxToolbarPanel::OnSelectionClear(const ee0::VariantSet& variants)
{
    m_node_prop->Clear();
}

}