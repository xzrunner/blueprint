#include "blueprint/WxNodeProperty.h"
#include "blueprint/ReflectPropTypes.h"

#include <ee0/SubjectMgr.h>
#include <ee0/ReflectPropTypes.h>
#include <ee0/WxPropHelper.h>
#include <ee0/MessageID.h>
#include <ee0/MsgHelper.h>
#include <blueprint/MessageID.h>
#include <blueprint/NodeHelper.h>
#include <blueprint/node/Function.h>

#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>

#include <wx/sizer.h>
#include <wx/propgrid/propgrid.h>

namespace bp
{

WxNodeProperty::WxNodeProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
    : wxPanel(parent)
    , m_sub_mgr(sub_mgr)
{
    InitLayout();
}

void WxNodeProperty::Clear()
{
    m_node.reset();
    m_obj.reset();

    m_pg->Clear();
}

void WxNodeProperty::LoadFromNode(const n0::SceneNodePtr& obj, const NodePtr& node)
{
    m_obj = obj;
	m_node = node;

	m_pg->Clear();

	auto node_type = node->get_type();
	for (auto& prop : node_type.get_properties())
	{
        auto name = prop.get_name().to_string();

		auto ui_info_obj = prop.get_metadata(ee0::UIMetaInfoTag());
		if (!ui_info_obj.is_valid()) {
			continue;
		}

		auto ui_info = ui_info_obj.get_value<ee0::UIMetaInfo>();
        auto prop_type = prop.get_type();

        if (prop_type == rttr::type::get<VariantType>())
        {
            const wxChar* TYPES[] = { wxT("Void"), wxT("Bool"), wxT("Int"),
                wxT("Float1"), wxT("Float2"), wxT("Float3"), wxT("Float4"), NULL };
            auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, TYPES);
            auto type = prop.get_value(node).get_value<VariantType>();
            type_prop->SetValue(static_cast<int>(type));
            m_pg->Append(type_prop);
        }
        else if (InitView(prop, node))
        {
            ;
        }
        else
        {
            ee0::WxPropHelper::CreateProp(m_pg, ui_info, node, prop, [&](const std::string& filepath)
            {
                if (m_node->get_type() == rttr::type::get<bp::node::Function>()) {
                    bp::NodeHelper::LoadFunctionNode(m_obj, m_node);
                }
                m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
                m_sub_mgr->NotifyObservers(bp::MSG_BP_NODE_PROP_CHANGED);
            });
        }
	}
}

void WxNodeProperty::InitLayout()
{
	auto sizer = new wxBoxSizer(wxVERTICAL);

	m_pg = new wxPropertyGrid(this, -1, wxDefaultPosition, wxSize(500, -1),
		wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED
	);
    Connect(m_pg->GetId(), wxEVT_PG_CHANGING, wxPropertyGridEventHandler(
        WxNodeProperty::OnPropertyGridChanging));
	Connect(m_pg->GetId(), wxEVT_PG_CHANGED, wxPropertyGridEventHandler(
		WxNodeProperty::OnPropertyGridChanged));
	sizer->Add(m_pg, wxEXPAND);

	SetSizer(sizer);
}

void WxNodeProperty::OnPropertyGridChanging(wxPropertyGridEvent& event)
{
    if (!m_node) {
        return;
    }

    wxPGProperty* property = event.GetProperty();
    auto key = property->GetName();
    wxAny val = property->GetValue();

    auto node_type = m_node->get_type();

    for (auto& prop : node_type.get_properties())
    {
        auto ui_info_obj = prop.get_metadata(ee0::UIMetaInfoTag());
        if (!ui_info_obj.is_valid()) {
            continue;
        }
        auto ui_info = ui_info_obj.get_value<ee0::UIMetaInfo>();
        if (prop.get_metadata(ee0::PropOpenFileTag()).is_valid() && key == ui_info.desc) {
            event.Veto();
            continue;
        }
    }
}

void WxNodeProperty::OnPropertyGridChanged(wxPropertyGridEvent& event)
{
	if (!m_node) {
		return;
	}

    bool dirty = UpdateView(event);

	wxPGProperty* property = event.GetProperty();
	auto key = property->GetName();
	wxAny val = property->GetValue();

	auto node_type = m_node->get_type();
	for (auto& prop : node_type.get_properties())
	{
        if (prop.get_metadata(ee0::PropOpenFileTag()).is_valid()) {
            continue;
        }
		auto ui_info_obj = prop.get_metadata(ee0::UIMetaInfoTag());
		if (!ui_info_obj.is_valid()) {
			continue;
		}
		auto ui_info = ui_info_obj.get_value<ee0::UIMetaInfo>();
        auto prop_type = prop.get_type();
        if (prop_type == rttr::type::get<VariantType>() && key == ui_info.desc)
        {
            prop.set_value(m_node, static_cast<VariantType>(wxANY_AS(val, int)));
            dirty = true;
        }
        else if (UpdateView(prop, *property))
        {
            dirty = true;
        }
        else if (ee0::WxPropHelper::UpdateProp(key, val, ui_info, m_node, prop))
        {
            dirty = true;
        }
	}

    if (dirty)
    {
        m_node->Refresh();

        m_node->SetEditNotDirty(false);

        // update aabb
        auto& st = m_node->GetStyle();
        m_obj->GetUniqueComp<n2::CompBoundingBox>().SetSize(
            *m_obj, sm::rect(st.width, st.height)
        );

        m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
        ee0::MsgHelper::SendObjMsg(*m_sub_mgr, m_obj, bp::MSG_BP_NODE_PROP_CHANGED);
    }
}

}