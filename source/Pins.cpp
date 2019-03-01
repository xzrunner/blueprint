#include "blueprint/Pins.h"
#include "blueprint/Connecting.h"

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

Pins::Pins(bool is_input, int pos, int type, const std::string& name, const Node& parent)
	: m_is_input(is_input)
	, m_pos(pos)
	, m_old_type(type)
	, m_new_type(type)
	, m_name(name)
	, m_parent(parent)
{
}

Pins::~Pins()
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

const pt0::Color& Pins::GetColor() const
{
	switch (m_new_type)
	{
	case PINS_PORT:
		return COL_PORT;
	case PINS_BOOLEAN:
		return COL_BOOLEAN;
	case PINS_INTEGER:
		return COL_INTEGER;
	case PINS_FLOAT:
		return COL_FLOAT;
	case PINS_STRING:
		return COL_STRING;
	case PINS_TEXT:
		return COL_TEXT;
	case PINS_VECTOR:
		return COL_VECTOR;
	case PINS_ROTATOR:
		return COL_ROTATOR;
	case PINS_TRANSFORM:
		return COL_TRANSFORM;
	case PINS_OBJECT:
		return COL_OBJECT;
	default:
		return COL_DEFAULT;
	}
}

bool Pins::CanTypeCast(const Pins& p) const
{
    auto type = p.GetType();
	if (m_new_type == type) {
		return true;
	}
    if (type == PINS_ANY_VAR || m_new_type == PINS_ANY_VAR) {
        return true;
    }

	switch (m_new_type)
	{
	case PINS_PORT:
		return false;
	case PINS_BOOLEAN:
		return false;
	case PINS_INTEGER:
		return type == PINS_BOOLEAN || type == PINS_FLOAT;
	case PINS_FLOAT:
		return type == PINS_BOOLEAN || type == PINS_INTEGER || type == PINS_VECTOR;
	case PINS_STRING:
		return false;
	case PINS_TEXT:
		return false;
	case PINS_VECTOR:
		return type == PINS_INTEGER || type == PINS_FLOAT;
	case PINS_ROTATOR:
		return false;
	case PINS_TRANSFORM:
		return false;
	case PINS_OBJECT:
		return false;
	}

	return false;
}

void Pins::SetType(int type)
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

void Pins::AddConnecting(const std::shared_ptr<Connecting>& conn)
{
	m_connecting.push_back(conn);
}

void Pins::RemoveConnecting(const std::shared_ptr<Connecting>& conn)
{
	for (auto& itr = m_connecting.begin(); itr != m_connecting.end(); ++itr) {
		if (*itr == conn) {
			m_connecting.erase(itr);
			break;
		}
	}
}

void Pins::ClearConnecting()
{
	m_connecting.clear();
}

}