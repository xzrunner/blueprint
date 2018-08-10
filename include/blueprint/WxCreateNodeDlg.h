#pragma once

#include <SM_Vector.h>

#include <wx/dialog.h>
#include <wx/treectrl.h>

namespace bp
{

class WxCreateNodeDlg : public wxDialog
{
public:
	WxCreateNodeDlg(wxWindow* parent, const wxPoint& pos);

	std::string GetSelectedType() const;

private:
	void InitLayout();

	void OnSelChanged(wxTreeEvent& event);
	void OnDoubleClick(wxTreeEvent& event);

private:
	wxTreeCtrl* m_tree;

}; // WxCreateNodeDlg

}