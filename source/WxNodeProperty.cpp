#include "blueprint/WxNodeProperty.h"
#include "blueprint/VarType.h"
#include "blueprint/PropDescImpl.h"

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
        auto& var = prop.var;
        switch (var.type)
        {
        case VarType::Bool:
        {
            auto c_prop = new wxBoolProperty(var.name, wxPG_LABEL, var.b);
            m_pg->Append(c_prop);
            m_pg->SetPropertyAttribute(c_prop, wxPG_BOOL_USE_CHECKBOX, true, wxPG_RECURSE);
        }
            break;
        case VarType::Int:
        {
            std::shared_ptr<PropEnum> p_enum = nullptr;
            for (auto& d : prop.descs) {
                if (d->GetType() == PropDesc::Type::Enum) {
                    p_enum = std::static_pointer_cast<PropEnum>(d);
                    break;
                }
            }
            if (p_enum)
            {
                wxArrayString choices;
                for (auto& t : p_enum->types) {
                    choices.push_back(t);
                }
                auto prop = new wxEnumProperty(var.name, wxPG_LABEL, choices);
                prop->SetValue(var.i);
                m_pg->Append(prop);
            }
            else
            {
                m_pg->Append(new wxIntProperty(var.name, wxPG_LABEL, var.i));
            }
        }
            break;
        case VarType::Float:
            m_pg->Append(new wxFloatProperty(var.name, wxPG_LABEL, var.f));
            break;
        case VarType::Float2:
        {
            auto c_prop = new wxStringProperty(var.name, wxPG_LABEL, wxT("<composed>"));
            m_pg->Append(c_prop);
            c_prop->SetExpanded(false);
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("X"), wxPG_LABEL, var.f2[0]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("Y"), wxPG_LABEL, var.f2[1]));
        }
            break;
        case VarType::Float3:
        {
            auto c_prop = new wxStringProperty(var.name, wxPG_LABEL, wxT("<composed>"));
            m_pg->Append(c_prop);
            c_prop->SetExpanded(false);
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("X"), wxPG_LABEL, var.f3[0]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("Y"), wxPG_LABEL, var.f3[1]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("Z"), wxPG_LABEL, var.f3[2]));
        }
            break;
        case VarType::Float4:
        {
            auto c_prop = new wxStringProperty(var.name, wxPG_LABEL, wxT("<composed>"));
            m_pg->Append(c_prop);
            c_prop->SetExpanded(false);
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("X"), wxPG_LABEL, var.f4[0]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("Y"), wxPG_LABEL, var.f4[1]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("Z"), wxPG_LABEL, var.f4[2]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("W"), wxPG_LABEL, var.f4[3]));
        }
            break;
        case VarType::Matrix2:
        {
            auto c_prop = new wxStringProperty(var.name, wxPG_LABEL, wxT("<composed>"));
            m_pg->Append(c_prop);
            c_prop->SetExpanded(false);
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m00"), wxPG_LABEL, var.m2[0]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m01"), wxPG_LABEL, var.m2[1]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m10"), wxPG_LABEL, var.m2[2]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m11"), wxPG_LABEL, var.m2[3]));
        }
            break;
        case VarType::Matrix3:
        {
            auto c_prop = new wxStringProperty(var.name, wxPG_LABEL, wxT("<composed>"));
            m_pg->Append(c_prop);
            c_prop->SetExpanded(false);
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m00"), wxPG_LABEL, var.m3[0]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m01"), wxPG_LABEL, var.m3[1]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m02"), wxPG_LABEL, var.m3[2]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m10"), wxPG_LABEL, var.m3[3]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m11"), wxPG_LABEL, var.m3[4]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m12"), wxPG_LABEL, var.m3[5]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m20"), wxPG_LABEL, var.m3[6]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m21"), wxPG_LABEL, var.m3[7]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m22"), wxPG_LABEL, var.m3[8]));
        }
            break;
        case VarType::Matrix4:
        {
            auto c_prop = new wxStringProperty(var.name, wxPG_LABEL, wxT("<composed>"));
            m_pg->Append(c_prop);
            c_prop->SetExpanded(false);
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m00"), wxPG_LABEL, var.m4[0]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m01"), wxPG_LABEL, var.m4[1]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m02"), wxPG_LABEL, var.m4[2]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m03"), wxPG_LABEL, var.m4[3]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m10"), wxPG_LABEL, var.m4[4]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m11"), wxPG_LABEL, var.m4[5]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m12"), wxPG_LABEL, var.m4[6]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m13"), wxPG_LABEL, var.m4[7]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m20"), wxPG_LABEL, var.m4[8]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m21"), wxPG_LABEL, var.m4[9]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m22"), wxPG_LABEL, var.m4[10]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m23"), wxPG_LABEL, var.m4[11]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m30"), wxPG_LABEL, var.m4[12]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m31"), wxPG_LABEL, var.m4[13]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m32"), wxPG_LABEL, var.m4[14]));
            m_pg->AppendIn(c_prop, new wxFloatProperty(wxT("m33"), wxPG_LABEL, var.m4[15]));
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
        auto& var = prop.var;
        if (var.name != key) {
            continue;
        }

        auto wx_prop = m_pg->GetProperty(var.name);
        assert(wx_prop);
        switch (var.type)
        {
        case VarType::Bool:
        {
            auto b = wxANY_AS(val, bool);
            const_cast<bp::Variant&>(var).b = b;
            wx_prop->SetValue(b);
        }
            break;
        case VarType::Int:
        {
            auto i = wxANY_AS(val, int);
            const_cast<bp::Variant&>(var).i = i;
            wx_prop->SetValue(i);
        }
            break;
        case VarType::Float:
        {
            auto f = wxANY_AS(val, float);
            const_cast<bp::Variant&>(var).f = f;
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
            memcpy(const_cast<bp::Variant&>(var).f2, xy, sizeof(float) * 2);
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
            memcpy(const_cast<bp::Variant&>(var).f3, xyz, sizeof(float) * 3);
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
            memcpy(const_cast<bp::Variant&>(var).f4, xyzw, sizeof(float) * 4);
            wx_prop->SetValue(str);
        }
            break;
        case VarType::Matrix2:
        {
            std::vector<std::string> tokens;
            auto str = wxANY_AS(val, wxString).ToStdString();
            cpputil::StringHelper::Split(str, ";", tokens);
            assert(tokens.size() == 4);

            float m2[4];
            for (int i = 0; i < 4; ++i) {
                m2[i] = std::stof(tokens[i]);
            }
            memcpy(const_cast<bp::Variant&>(var).m2, m2, sizeof(m2));
            wx_prop->SetValue(str);
        }
            break;
        case VarType::Matrix3:
        {
            std::vector<std::string> tokens;
            auto str = wxANY_AS(val, wxString).ToStdString();
            cpputil::StringHelper::Split(str, ";", tokens);
            assert(tokens.size() == 9);

            float m3[9];
            for (int i = 0; i < 9; ++i) {
                m3[i] = std::stof(tokens[i]);
            }
            memcpy(const_cast<bp::Variant&>(var).m3, m3, sizeof(m3));
            wx_prop->SetValue(str);
        }
            break;
        case VarType::Matrix4:
        {
            std::vector<std::string> tokens;
            auto str = wxANY_AS(val, wxString).ToStdString();
            cpputil::StringHelper::Split(str, ";", tokens);
            assert(tokens.size() == 16);

            float m4[16];
            for (int i = 0; i < 16; ++i) {
                m4[i] = std::stof(tokens[i]);
            }
            memcpy(const_cast<bp::Variant&>(var).m4, m4, sizeof(m4));
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