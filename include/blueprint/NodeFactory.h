#pragma once

#include <memory>
#include <string>

namespace bp
{
namespace node
{

class Node;

class NodeFactory
{
public:
	static std::shared_ptr<Node> Create(const std::string& type);

}; // NodeFactory

}
}