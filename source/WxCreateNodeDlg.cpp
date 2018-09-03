#include "blueprint/WxCreateNodeDlg.h"
#include "blueprint/Node.h"
#include "blueprint/Pins.h"

#include <wx/treectrl.h>
#include <wx/sizer.h>

namespace bp
{

WxCreateNodeDlg::WxCreateNodeDlg(wxWindow* parent, const wxPoint& pos, const Pins& pair,
	                             const std::vector<std::shared_ptr<Node>>& nodes)
	: wxDialog(parent, wxID_ANY, "Create Node", pos, wxSize(200, 400))
	, m_pair(pair)
	, m_nodes(nodes)
{
	InitLayout();
}

std::string WxCreateNodeDlg::GetSelectedType() const
{
	return m_tree->GetItemText(m_tree->GetSelection()).ToStdString();
}

void WxCreateNodeDlg::InitLayout()
{
	wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
	{
		m_tree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxTR_HIDE_ROOT | /*wxTR_NO_LINES | */wxTR_DEFAULT_STYLE);
		Bind(wxEVT_TREE_SEL_CHANGED, &WxCreateNodeDlg::OnSelChanged, this, m_tree->GetId());
		Bind(wxEVT_TREE_ITEM_ACTIVATED, &WxCreateNodeDlg::OnDoubleClick, this, m_tree->GetId());

		auto root = m_tree->AddRoot("ROOT");

		int idx = 0;
		for (auto& node : m_nodes) {
			if (IsNodeMatched(*node)) {
				m_tree->InsertItem(root, idx++, node->TypeName());
			}
		}

		top_sizer->Add(m_tree, 1, wxEXPAND);
	}
	SetSizer(top_sizer);
//	top_sizer->Fit(this);
}

void WxCreateNodeDlg::OnSelChanged(wxTreeEvent& event)
{
	//assert(IsModal());
	//EndModal(wxID_OK);
}

void WxCreateNodeDlg::OnDoubleClick(wxTreeEvent& event)
{
	assert(IsModal());
	EndModal(wxID_OK);
}

bool WxCreateNodeDlg::IsNodeMatched(const Node& node) const
{
	auto pins_type = m_pair.GetType();
	if (m_pair.IsInput())
	{
		auto& output = node.GetAllOutput();
		for (auto& pins : output) {
			if (pins->CanTypeCast(pins_type)) {
				return true;
			}
		}
	} else {
		auto& input = node.GetAllInput();
		for (auto& pins : input) {
			if (pins->CanTypeCast(pins_type)) {
				return true;
			}
		}
	}
	return false;
}

}