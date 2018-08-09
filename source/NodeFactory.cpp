#include "blueprint/NodeFactory.h"
// flow control
#include "blueprint/FlowCtrlBranch.h"
// event
#include "blueprint/EventBeginPlay.h"

namespace bp
{
namespace node
{

std::shared_ptr<Node> NodeFactory::Create(const std::string& type)
{
	std::shared_ptr<Node> node = nullptr;
	// flow control
	if (type == "flow_ctrl_branch") {
		node = std::make_shared<FlowCtrlBranch>();
	}
	// event
	else if (type == "event_begin_play") {
		node = std::make_shared<EventBeginPlay>();
	}
	return node;
}

}
}