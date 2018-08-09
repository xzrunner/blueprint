#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

Pins::Pins(PinsType type, const std::string& name, const Node& parent)
	: m_type(type)
	, m_name(name)
	, m_parent(parent) 
{
}

}
}