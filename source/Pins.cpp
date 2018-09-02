#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

Pins::Pins(bool is_input, int pos, int type, const std::string& name,
	       const Node& parent, bool type_cast)
	: m_is_input(is_input)
	, m_pos(pos)
	, m_type(type)
	, m_name(name)
	, m_parent(parent)
	, m_type_cast(type_cast)
{
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