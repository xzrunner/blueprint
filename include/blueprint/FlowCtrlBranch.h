#pragma once

#include "blueprint/Node.h"

namespace bp
{
namespace node
{

class FlowCtrlBranch : public Node
{
public:
	FlowCtrlBranch();

	virtual NodeTypeID TypeID() const override {
		return GetNodeTypeID<FlowCtrlBranch>();
	}
	virtual const std::string& TypeName() const override {
		return TYPE_NAME;
	}
	virtual std::shared_ptr<Node> Create() const override {
		return std::make_shared<FlowCtrlBranch>();
	}

	static const std::string TYPE_NAME;

private:
	// input
	std::shared_ptr<Pins> m_input;
	std::shared_ptr<Pins> m_condition;

	// output
	std::shared_ptr<Pins> m_true;
	std::shared_ptr<Pins> m_false;

}; // FlowCtrlBranch

}
}