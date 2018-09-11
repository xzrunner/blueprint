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

NodePtr NodeFactory::Create(const std::string& type, const std::string& name)
{
	for (auto& n : m_nodes)
	{
		if (n->TypeName() != type) {
			continue;
		}
		auto node = n->Create();
		if (!name.empty()) {
			node->SetName(name);
		}
		return node;
	}
	return nullptr;
}

void NodeFactory::RegistNodes(const std::vector<NodePtr>& nodes)
{
	std::copy(nodes.begin(), nodes.end(), std::back_inserter(m_nodes));
}

void NodeFactory::RegistAllNode()
{
	m_nodes.push_back(std::make_shared<node::EventBeginPlay>());
	m_nodes.push_back(std::make_shared<node::FlowCtrlBranch>());
}

}