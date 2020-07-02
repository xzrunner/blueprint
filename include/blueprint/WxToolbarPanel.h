#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/panel.h>

namespace ur { class Device; }
namespace ee0 { class WxNavigationBar; }

namespace bp
{

class WxNodeProperty;

class WxToolbarPanel : public wxPanel, public ee0::Observer
{
public:
    WxToolbarPanel(const ur::Device& dev, wxWindow* parent,
        const ee0::SubjectMgrPtr& graph_sub_mgr, bool nav_bar = false);
    virtual ~WxToolbarPanel();

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    void PushDefaultProp(wxPanel* default_prop);

private:
    void InitLayout(const ur::Device& dev, bool nav_bar);

    void OnSelectionInsert(const ee0::VariantSet& variants);
    void OnSelectionClear(const ee0::VariantSet& variants);

    void OnRootToNextLevel(const ee0::VariantSet& variants);

private:
    ee0::SubjectMgrPtr m_graph_sub_mgr = nullptr;

    wxPanel* m_default_prop = nullptr;
    WxNodeProperty* m_node_prop;

    ee0::WxNavigationBar* m_nav_bar = nullptr;

}; // WxToolbarPanel

}