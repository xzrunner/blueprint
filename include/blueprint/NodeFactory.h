#pragma once

#include "blueprint/typedef.h"

#include <cu/cu_macro.h>
#include <cpputil/ClassInfo.h>

#include <string>
#include <vector>
#include <map>

namespace bp
{

class Node;

class NodeFactory
{
public:
	void Register(cpputil::ClassInfo<Node>* ci);

	NodePtr Create(const std::string& class_name,
		const std::string& node_name = "");

	const std::vector<NodePtr>& GetAllNodes() const;

private:
	std::map<std::string, cpputil::ClassInfo<bp::Node>*> m_class_info_map;

	mutable std::vector<NodePtr> m_nodes;

	CU_SINGLETON_DECLARATION(NodeFactory)

}; // NodeFactory

}