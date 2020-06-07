#pragma once

#include "blueprint/typedef.h"

#include <SM_Vector.h>

#include <wx/dialog.h>
#include <wx/treectrl.h>

#include <map>

namespace bp
{

class Pin;
class Node;

class WxCreateNodeDlg : public wxDialog
{
public:
	WxCreateNodeDlg(wxWindow* parent, const wxPoint& pos, const std::shared_ptr<Pin>& pair,
        const std::vector<NodePtr>& nodes, std::function<bool(const bp::Node&, const bp::Pin&)> is_port_matched = nullptr);

	std::string GetSelectedType() const;

private:
	void InitLayout();

	void OnSelChanged(wxTreeEvent& event);
	void OnDoubleClick(wxTreeEvent& event);

	bool IsNodeMatched(const Node& node) const;

private:
    struct Item
    {
        Item(const std::string& name, const std::string& type, const std::string& group)
            : name(name), type(type), group(group) {}

        std::string name, type, group;
        std::vector<Item> children;
    };

    std::vector<Item> NodesToGroups() const;
    void InsertItem(const wxTreeItemId& parent, const Item& item, int idx);

private:
	std::shared_ptr<Pin> m_pair = nullptr;
	const std::vector<NodePtr>& m_nodes;

	wxTreeCtrl* m_tree;

    std::map<std::string, std::string> m_name2type;

    std::function<bool(const bp::Node&, const bp::Pin&)> m_is_port_matched = nullptr;

}; // WxCreateNodeDlg

}