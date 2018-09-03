#pragma once

#include <SM_Vector.h>

#include <wx/dialog.h>
#include <wx/treectrl.h>

namespace bp
{

class Pins;
class Node;

class WxCreateNodeDlg : public wxDialog
{
public:
	WxCreateNodeDlg(wxWindow* parent, const wxPoint& pos, const Pins& pair,
		const std::vector<std::shared_ptr<Node>>& nodes);

	std::string GetSelectedType() const;

private:
	void InitLayout();

	void OnSelChanged(wxTreeEvent& event);
	void OnDoubleClick(wxTreeEvent& event);

	bool IsNodeMatched(const Node& node) const;

private:
	const Pins& m_pair;
	const std::vector<std::shared_ptr<Node>>& m_nodes;

	wxTreeCtrl* m_tree;

}; // WxCreateNodeDlg

}