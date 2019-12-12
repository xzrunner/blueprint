#pragma once

#include "blueprint/Node.h"

#include <ee0/typedef.h>

#include <node0/typedef.h>

#include <wx/panel.h>

class wxPropertyGrid;
class wxPropertyGridEvent;
class wxPGProperty;
class wxEnumProperty;

namespace bp
{

class WxNodeProperty : public wxPanel
{
public:
    WxNodeProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr);

    virtual void LoadFromNode(const n0::SceneNodePtr& obj, const NodePtr& node);

    void Clear();

protected:
    virtual bool InitView(const rttr::property& prop, const NodePtr& node) { return false; }
    virtual bool UpdateView(const rttr::property& prop, const wxPGProperty& wx_prop) { return false; }
    virtual bool UpdateView(wxPropertyGridEvent& event) { return false; }

    static wxEnumProperty* CreateEnumProp(const std::string& label, rttr::type type, int init_val);
    static rttr::variant QueryEnumPropByLabel(const std::string& label, rttr::type type);

private:
    void InitLayout();

    virtual void OnPropertyGridChanging(wxPropertyGridEvent& event);
    virtual void OnPropertyGridChanged(wxPropertyGridEvent& event);

protected:
    wxPropertyGrid* m_pg;

    NodePtr m_node = nullptr;

    ee0::SubjectMgrPtr m_sub_mgr;

private:
    n0::SceneNodePtr m_obj  = nullptr;

}; // WxNodeProperty

}