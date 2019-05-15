#include "blueprint/Pin.h"
#include "blueprint/Connecting.h"
#include "blueprint/Node.h"
#include "blueprint/Pin.h"
#include "blueprint/node/Hub.h"

namespace
{

const pt0::Color COL_DEFAULT   = pt0::Color(255, 255, 255);

const pt0::Color COL_PORT      = pt0::Color(255, 255, 255);
const pt0::Color COL_BOOLEAN   = pt0::Color(140, 0, 0);
const pt0::Color COL_INTEGER   = pt0::Color(79, 225, 174);
const pt0::Color COL_FLOAT     = pt0::Color(168, 255, 81);
const pt0::Color COL_STRING    = pt0::Color(241, 0, 205);
const pt0::Color COL_TEXT      = pt0::Color(221, 119, 164);
const pt0::Color COL_VECTOR    = pt0::Color(247, 199, 45);
const pt0::Color COL_ROTATOR   = pt0::Color(160, 175, 250);
const pt0::Color COL_TRANSFORM = pt0::Color(243, 111, 0);
const pt0::Color COL_OBJECT    = pt0::Color(56, 165, 241);

}

namespace bp
{

Pin::ExtendFuncs Pin::m_funcs;

Pin::Pin(bool is_input, int pos, int type, const std::string& name, const Node& parent)
	: m_is_input(is_input)
	, m_pos(pos)
	, m_old_type(type)
	, m_new_type(type)
	, m_name(name)
	, m_parent(parent)
{
}

Pin::~Pin()
{
	for (auto& conn : m_connecting)
	{
		if (auto from = conn->GetFrom()) {
			from->RemoveConnecting(conn);
		}
		if (auto to = conn->GetTo()) {
			to->RemoveConnecting(conn);
		}
	}
}

std::string Pin::GetDesc() const
{
    if (m_funcs.get_desc_func) {
        return m_funcs.get_desc_func(GetName(), GetType());
    }

    return m_name;
}

const pt0::Color& Pin::GetColor() const
{
    if (m_funcs.get_color_func) {
        return m_funcs.get_color_func(GetType());
    }

	switch (m_new_type)
	{
	case PIN_PORT:
		return COL_PORT;
	case PIN_BOOLEAN:
		return COL_BOOLEAN;
	case PIN_INTEGER:
		return COL_INTEGER;
	case PIN_FLOAT1:
    case PIN_FLOAT2:
    case PIN_FLOAT3:
    case PIN_FLOAT4:
		return COL_FLOAT;
	case PIN_STRING:
		return COL_STRING;
	case PIN_TEXT:
		return COL_TEXT;
	case PIN_VECTOR:
		return COL_VECTOR;
	case PIN_ROTATOR:
		return COL_ROTATOR;
	case PIN_TRANSFORM:
		return COL_TRANSFORM;
	case PIN_OBJECT:
		return COL_OBJECT;
	default:
		return COL_DEFAULT;
	}
}

bool Pin::CanTypeCast(const Pin& p) const
{
    if (p.GetParent().get_type() == rttr::type::get<node::Hub>() &&
        p.IsInput())
    {
        auto& conns = p.GetParent().GetAllOutput()[0]->GetConnecting();
        if (!conns.empty()) {
            return CanTypeCast(*conns[0]->GetTo());
        }
    }

    if (m_funcs.can_type_cast_func) {
        return m_funcs.can_type_cast_func(GetType(), p.GetType());
    }

    auto type = p.GetType();
	if (m_new_type == type) {
		return true;
	}
    if (type == PIN_ANY_VAR || m_new_type == PIN_ANY_VAR) {
        return true;
    }

	switch (m_new_type)
	{
	case PIN_PORT:
		return false;
	case PIN_BOOLEAN:
		return false;
	case PIN_INTEGER:
		return type == PIN_BOOLEAN || type == PIN_FLOAT1;
	case PIN_FLOAT1:
		return type == PIN_BOOLEAN || type == PIN_INTEGER || type == PIN_VECTOR;
    case PIN_FLOAT2:
    case PIN_FLOAT3:
    case PIN_FLOAT4:
        return type == PIN_VECTOR;
	case PIN_STRING:
		return false;
	case PIN_TEXT:
		return false;
	case PIN_VECTOR:
		return type == PIN_INTEGER || type == PIN_FLOAT1 || type == PIN_FLOAT2 || type == PIN_FLOAT3 || type == PIN_FLOAT4;
	case PIN_ROTATOR:
		return false;
	case PIN_TRANSFORM:
		return false;
	case PIN_OBJECT:
		return false;
	}

	return false;
}

void Pin::SetType(int type)
{
	if (m_new_type == type) {
		return;
	}

	m_new_type = type;

	// update curve color
	for (auto& conn : m_connecting) {
		conn->UpdateCurve();
	}
}

void Pin::AddConnecting(const std::shared_ptr<Connecting>& conn)
{
	m_connecting.push_back(conn);
}

void Pin::RemoveConnecting(const std::shared_ptr<Connecting>& conn)
{
	for (auto& itr = m_connecting.begin(); itr != m_connecting.end(); ++itr) {
		if (*itr == conn) {
			m_connecting.erase(itr);
			break;
		}
	}
}

void Pin::ClearConnecting()
{
	m_connecting.clear();
}

}