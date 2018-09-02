#include "blueprint/Pins.h"

namespace
{

const pt2::Color COL_DEFAULT   = pt2::Color(255, 255, 255);

const pt2::Color COL_PORT      = pt2::Color(255, 255, 255);
const pt2::Color COL_BOOLEAN   = pt2::Color(140, 0, 0);
const pt2::Color COL_INTEGER   = pt2::Color(79, 225, 174);
const pt2::Color COL_FLOAT     = pt2::Color(168, 255, 81);
const pt2::Color COL_STRING    = pt2::Color(241, 0, 205);
const pt2::Color COL_TEXT      = pt2::Color(221, 119, 164);
const pt2::Color COL_VECTOR    = pt2::Color(247, 199, 45);
const pt2::Color COL_ROTATOR   = pt2::Color(160, 175, 250);
const pt2::Color COL_TRANSFORM = pt2::Color(243, 111, 0);
const pt2::Color COL_OBJECT    = pt2::Color(56, 165, 241);

}

namespace bp
{
namespace node
{

Pins::Pins(bool is_input, int pos, int type, const std::string& name, const Node& parent)
	: m_is_input(is_input)
	, m_pos(pos)
	, m_type(type)
	, m_name(name)
	, m_parent(parent)
{
}

const pt2::Color& Pins::GetColor() const
{
	switch (m_type)
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

bool Pins::CanTypeCast(int type) const
{
	if (m_type == type) {
		return true;
	}

	switch (m_type)
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
}