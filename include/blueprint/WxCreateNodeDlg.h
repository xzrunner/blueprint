#pragma once

#include "blueprint/typedef.h"

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
		const std::vector<NodePtr>& nodes);

	std::string GetSelectedType() const;

private:
	void InitLayout();

	void OnSelChanged(wxTreeEvent& event);
	void OnDoubleClick(wxTreeEvent& event);

	bool IsNodeMatched(const Node& node) const;

private:
	const Pins& m_pair;
	const std::vector<NodePtr>& m_nodes;

	wxTreeCtrl* m_tree;

}; // WxCreateNodeDlg

}