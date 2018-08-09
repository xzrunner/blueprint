#pragma once

#include <string>
#include <memory>

namespace bp
{
namespace node
{

class Node;

enum PinsType
{
	PINS_PORT = 0,

	// variable
	PINS_BOOLEAN,
	PINS_INTEGER,
	PINS_FLOAT,
	PINS_STRING,
	PINS_TEXT,
	PINS_VECTOR,
	PINS_ROTATOR,
	PINS_TRANSFORM,
	PINS_OBJECT,
};

class Pins
{
public:
	Pins(PinsType type, const std::string& name, const Node& parent);

	PinsType GetType() const { return m_type; }

	auto& GetName() const { return m_name; }

	auto& GetConnected() const { return m_connected; }

private:
	PinsType m_type;

	std::string m_name;

	const Node& m_parent;

	std::shared_ptr<Pins> m_connected = nullptr;

}; // Pins

}
}