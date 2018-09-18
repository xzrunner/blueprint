#include "blueprint/NodeFactory.h"
#include "blueprint/Node.h"

namespace bp
{

CU_SINGLETON_DEFINITION(NodeFactory);

NodeFactory::NodeFactory()
{
}

void NodeFactory::Register(cpputil::ClassInfo<Node>* ci)
{
	auto& name = ci->GetClassName();
	if (m_class_info_map.find(name) == m_class_info_map.end()) {
		m_class_info_map.insert({ name, ci });
	}
}

NodePtr NodeFactory::Create(const std::string& class_name, const std::string& node_name)
{
	auto itr = m_class_info_map.find(class_name);
	if (itr == m_class_info_map.end()) {
		return nullptr;
	}

	auto node = itr->second->GetCtor()();
	if (!node_name.empty()) {
		node->SetName(node_name);
	}
	return node;
}

const std::vector<NodePtr>& NodeFactory::GetAllNodes() const
{
	if (m_nodes.size() != m_class_info_map.size())
	{
		m_nodes.clear();
		m_nodes.reserve(m_class_info_map.size());
		for (auto& itr : m_class_info_map) {
			m_nodes.push_back(itr.second->GetCtor()());
		}
	}

	return m_nodes;
}

}