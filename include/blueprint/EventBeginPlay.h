#pragma once

#include "blueprint/Node.h"

namespace bp
{
namespace node
{

class EventBeginPlay : public Node
{
public:
	EventBeginPlay();

	virtual std::string TypeName() const override {
		return "event_begin_play";
	}
	virtual std::shared_ptr<Node> Create() const override {
		return std::make_shared<EventBeginPlay>();
	}

private:
	std::shared_ptr<Pins> m_output;

}; // EventBeginPlay

}
}