#pragma once

#include <painting2/BezierShape.h>

#include <string>
#include <memory>

namespace bp
{
namespace node
{

class Node;
class Connecting;

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
	Pins(bool is_input, int pos, PinsType type,
		const std::string& name, const Node& parent);

	bool IsInput() const { return m_is_input; }
	int  GetPosIdx() const { return m_pos; }

	PinsType GetType() const { return m_type; }

	auto& GetName() const { return m_name; }

	auto& GetParent() const { return m_parent; }

	void AddConnecting(const std::shared_ptr<Connecting>& conn);
	void RemoveConnecting(const std::shared_ptr<Connecting>& conn);
	void ClearConnecting();

	auto& GetConnecting() const { return m_connecting; }

private:
	// for parent
	bool m_is_input;
	int  m_pos;

	PinsType m_type;

	std::string m_name;

	const Node& m_parent;

	std::vector<std::shared_ptr<Connecting>> m_connecting;

}; // Pins

}
}