#pragma once

#include "blueprint/typedef.h"

#include <cu/cu_macro.h>

#include <memory>
#include <string>
#include <vector>

namespace bp
{

class NodeFactory
{
public:
	NodePtr Create(const std::string& type);

	auto& GetAllNodes() const { return m_nodes; }

	void RegistNodes(const std::vector<NodePtr>& nodes);

private:
	void RegistAllNode();

private:
	std::vector<NodePtr> m_nodes;

	CU_SINGLETON_DECLARATION(NodeFactory)

}; // NodeFactory

}