#include "blueprint/WxCreateNodeDlg.h"
#include "blueprint/Node.h"
#include "blueprint/Pin.h"

#include <wx/treectrl.h>
#include <wx/sizer.h>

namespace bp
{

WxCreateNodeDlg::WxCreateNodeDlg(wxWindow* parent, const wxPoint& pos,
                                 const std::shared_ptr<Pin>& pair,
	                             const std::vector<NodePtr>& nodes)
	: wxDialog(parent, wxID_ANY, "Create Node", pos, wxSize(200, 400))
	, m_pair(pair)
	, m_nodes(nodes)
{
	InitLayout();
}

std::string WxCreateNodeDlg::GetSelectedType() const
{
    auto name = m_tree->GetItemText(m_tree->GetSelection()).ToStdString();
    auto itr = m_name2type.find(name);
    assert(itr != m_name2type.end());
    return itr->second;
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
		for (auto& node : m_nodes)
        {
            if (!IsNodeMatched(*node)) {
                continue;
            }

            auto& title = node->GetTitle();
            auto type = node->get_type().get_name().to_string();
            std::string name;
            if (title.empty()) {
                name = type;
            } else {
                auto pos = type.find_last_of("::");
                if (pos != std::string::npos) {
                    std::string prefix = type.substr(0, type.find_first_of("::") + 2);
                    name = prefix + title;
                } else {
                    name = title;
                }
            }

            m_name2type.insert({ name, type });
            m_tree->InsertItem(root, idx++, name);
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
    if (!m_pair) {
        return true;
    }

	if (m_pair->IsInput())
	{
		auto& output = node.GetAllOutput();
		for (auto& pin : output) {
			if (pin->CanTypeCast(*m_pair)) {
				return true;
			}
		}
	} else {
		auto& input = node.GetAllInput();
		for (auto& pin : input) {
			if (pin->CanTypeCast(*m_pair)) {
				return true;
			}
		}
	}
	return false;
}

}