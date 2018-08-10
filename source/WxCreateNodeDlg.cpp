#include "blueprint/WxCreateNodeDlg.h"
#include "blueprint/NodeFactory.h"
#include "blueprint/Node.h"

#include <wx/treectrl.h>
#include <wx/sizer.h>

namespace bp
{

WxCreateNodeDlg::WxCreateNodeDlg(wxWindow* parent, const wxPoint& pos)
	: wxDialog(parent, wxID_ANY, "Create Node", pos, wxSize(200, 400))
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

		auto& nodes = node::NodeFactory::Instance()->GetAllNodes();
		int idx = 0;
		for (auto& node : nodes) {
			m_tree->InsertItem(root, idx++, node->TypeName());
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

}