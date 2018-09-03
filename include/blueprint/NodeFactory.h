#pragma once

#include <cu/cu_macro.h>

#include <memory>
#include <string>
#include <vector>

namespace bp
{

class Node;

class NodeFactory
{
public:
	std::shared_ptr<Node> Create(const std::string& type);

	auto& GetAllNodes() const { return m_nodes; }

private:
	void RegistAllNode();

private:
	std::vector<std::shared_ptr<Node>> m_nodes;

	CU_SINGLETON_DECLARATION(NodeFactory)

}; // NodeFactory

}