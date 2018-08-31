#pragma once

#include <SM_Vector.h>

#include <wx/dialog.h>
#include <wx/treectrl.h>

namespace bp
{

namespace node { class Pins; class Node; }

class WxCreateNodeDlg : public wxDialog
{
public:
	WxCreateNodeDlg(wxWindow* parent, const wxPoint& pos, const node::Pins& pair,
		const std::vector<std::shared_ptr<node::Node>>& nodes);

	std::string GetSelectedType() const;

private:
	void InitLayout();

	void OnSelChanged(wxTreeEvent& event);
	void OnDoubleClick(wxTreeEvent& event);

	bool IsNodeMatched(const node::Node& node) const;

private:
	const node::Pins& m_pair;
	const std::vector<std::shared_ptr<node::Node>>& m_nodes;

	wxTreeCtrl* m_tree;

}; // WxCreateNodeDlg

}