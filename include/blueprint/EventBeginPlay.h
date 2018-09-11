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

	virtual NodeTypeID TypeID() const override {
		return GetNodeTypeID<EventBeginPlay>();
	}
	virtual const std::string& TypeName() const override {
		return TYPE_NAME;
	}
	virtual NodePtr Create() const override {
		return std::make_shared<EventBeginPlay>();
	}

	static const std::string TYPE_NAME;

private:
	std::shared_ptr<Pins> m_output;

}; // EventBeginPlay

}
}