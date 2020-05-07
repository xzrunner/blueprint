#include "blueprint/WxNodeProperty.h"
#include "blueprint/VarType.h"

#include <ee0/SubjectMgr.h>
#include <ee0/ReflectPropTypes.h>
#include <ee0/WxPropHelper.h>
#include <ee0/MessageID.h>
#include <ee0/MsgHelper.h>
#include <blueprint/MessageID.h>
#include <blueprint/NodeHelper.h>
#include <blueprint/node/Function.h>

#include <cpputil/StringHelper.h>
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>

#include <wx/sizer.h>
#include <wx/propgrid/propgrid.h>

namespace bp
{

WxNodeProperty::WxNodeProperty(const ur::Device& dev, wxWindow* parent,
                               const ee0::SubjectMgrPtr& sub_mgr)
    : wxPanel(parent)
    , m_dev(dev)
    , m_sub_mgr(sub_mgr)
{
    InitLayout();
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

        if (prop_type == rttr::type::get<VarType>())
        {
            const wxChar* TYPES[] = { wxT("Void"), wxT("Bool"), wxT("Int"),
                wxT("Float1"), wxT("Float2"), wxT("Float3"), wxT("Float4"), NULL };
            auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, TYPES);
            auto type = prop.get_value(node).get_value<VarType>();
            type_prop->SetValue(static_cast<int>(type));
            m_pg->Append(type_prop);
        }
        else if (InitView(prop, node))
        {
            ;
        }
        else
        {
            ee0::WxPropHelper::CreateProp(m_pg, ui_info, node, prop,
            [&](const std::string& filepath)
            {
                if (m_node->get_type() == rttr::type::get<bp::node::Function>()) {
                    bp::NodeHelper::LoadFunctionNode(m_dev, m_obj, m_node);
                }
                m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
                ee0::MsgHelper::SendObjMsg(*m_sub_mgr, m_obj, bp::MSG_BP_NODE_PROP_CHANGED);
            },
            [&](const std::string& code_str)
            {
                m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
                ee0::MsgHelper::SendObjMsg(*m_sub_mgr, m_obj, bp::MSG_BP_NODE_PROP_CHANGED);
            }
            );
        }
	}

    for (auto& prop : node->GetProps())
    {
        switch (prop.type)
        {
        case VarType::Bool:
        {
            auto c_prop = new wxBoolProperty(prop.name, wxPG_LABEL, prop.b);
            m_pg->Append(c_prop);
            m_pg->SetPropertyAttribute(c_prop, wxPG_BOOL_USE_CHECKBOX, true, wxPG_RECURSE);
        }
            break;
        case VarType::Int:
            m_pg->Append(new wxIntProperty(prop.name, wxPG_LABEL, prop.i));
            break;
        case VarType::Float:
            m_pg->Append(new wxFloatProperty(prop.name, wxPG_LABEL, prop.f));
            break;
        case VarType::Float2:
        {
            auto c_prop = new wxStringProperty(prop.name, wxPG_LABEL, wxT("<composed>"));
            m_pg->Append(c_prop);
            c_prop->SetExpanded(false);
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("X"), wxPG_LABEL, prop.f2[0]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("Y"), wxPG_LABEL, prop.f2[1]));
        }
            break;
        case VarType::Float3:
        {
            auto c_prop = new wxStringProperty(prop.name, wxPG_LABEL, wxT("<composed>"));
            m_pg->Append(c_prop);
            c_prop->SetExpanded(false);
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("X"), wxPG_LABEL, prop.f3[0]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("Y"), wxPG_LABEL, prop.f3[1]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("Z"), wxPG_LABEL, prop.f3[2]));
        }
            break;
        case VarType::Float4:
        {
            auto c_prop = new wxStringProperty(prop.name, wxPG_LABEL, wxT("<composed>"));
            m_pg->Append(c_prop);
            c_prop->SetExpanded(false);
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("X"), wxPG_LABEL, prop.f4[0]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("Y"), wxPG_LABEL, prop.f4[1]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("Z"), wxPG_LABEL, prop.f4[2]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("W"), wxPG_LABEL, prop.f4[3]));
        }
            break;
        default:
            assert(0);
        }
    }
}

void WxNodeProperty::Clear()
{
    m_node.reset();
    m_obj.reset();

    m_pg->Clear();
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
        if (prop_type == rttr::type::get<VarType>() && key == ui_info.desc)
        {
            prop.set_value(m_node, static_cast<VarType>(wxANY_AS(val, int)));
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

    for (auto& prop : m_node->GetProps())
    {
        if (prop.name != key) {
            continue;
        }

        auto wx_prop = m_pg->GetProperty(prop.name);
        assert(wx_prop);
        switch (prop.type)
        {
        case VarType::Bool:
        {
            auto b = wxANY_AS(val, bool);
            const_cast<bp::Variant&>(prop).b = b;
            wx_prop->SetValue(b);
        }
            break;
        case VarType::Int:
        {
            auto i = wxANY_AS(val, int);
            const_cast<bp::Variant&>(prop).i = i;
            wx_prop->SetValue(i);
        }
            break;
        case VarType::Float:
        {
            auto f = wxANY_AS(val, float);
            const_cast<bp::Variant&>(prop).f = f;
            wx_prop->SetValue(f);
        }
            break;
        case VarType::Float2:
        {
            std::vector<std::string> tokens;
            auto str = wxANY_AS(val, wxString).ToStdString();
            cpputil::StringHelper::Split(str, ";", tokens);
            assert(tokens.size() == 2);

            float xy[2];
            xy[0] = std::stof(tokens[0]);
            xy[1] = std::stof(tokens[1]);
            memcpy(const_cast<bp::Variant&>(prop).f2, xy, sizeof(float) * 2);
            wx_prop->SetValue(str);
        }
            break;
        case VarType::Float3:
        {
            std::vector<std::string> tokens;
            auto str = wxANY_AS(val, wxString).ToStdString();
            cpputil::StringHelper::Split(str, ";", tokens);
            assert(tokens.size() == 3);

            float xyz[3];
            xyz[0] = std::stof(tokens[0]);
            xyz[1] = std::stof(tokens[1]);
            xyz[2] = std::stof(tokens[2]);
            memcpy(const_cast<bp::Variant&>(prop).f3, xyz, sizeof(float) * 3);
            wx_prop->SetValue(str);
        }
            break;
        case VarType::Float4:
        {
            std::vector<std::string> tokens;
            auto str = wxANY_AS(val, wxString).ToStdString();
            cpputil::StringHelper::Split(str, ";", tokens);
            assert(tokens.size() == 4);

            float xyzw[4];
            xyzw[0] = std::stof(tokens[0]);
            xyzw[1] = std::stof(tokens[1]);
            xyzw[2] = std::stof(tokens[2]);
            xyzw[3] = std::stof(tokens[3]);
            memcpy(const_cast<bp::Variant&>(prop).f4, xyzw, sizeof(float) * 4);
            wx_prop->SetValue(str);
        }
            break;
        default:
            assert(0);
        }
        dirty = true;
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