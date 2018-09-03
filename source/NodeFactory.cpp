#include "blueprint/NodeFactory.h"
// flow control
#include "blueprint/FlowCtrlBranch.h"
// event
#include "blueprint/EventBeginPlay.h"

namespace bp
{

CU_SINGLETON_DEFINITION(NodeFactory);

NodeFactory::NodeFactory()
{
	RegistAllNode();
}

std::shared_ptr<Node> NodeFactory::Create(const std::string& type)
{
	for (auto& n : m_nodes) {
		if (n->TypeName() == type) {
			return n->Create();
		}
	}
	return nullptr;
}

void NodeFactory::RegistNodes(const std::vector<std::shared_ptr<Node>>& nodes)
{
	std::copy(nodes.begin(), nodes.end(), std::back_inserter(m_nodes));
}

void NodeFactory::RegistAllNode()
{
	m_nodes.push_back(std::make_shared<node::EventBeginPlay>());
	m_nodes.push_back(std::make_shared<node::FlowCtrlBranch>());
}

}