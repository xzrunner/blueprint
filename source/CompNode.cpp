#include "blueprint/CompNode.h"

namespace bp
{

const char* const CompNode::TYPE_NAME = "bp_node";

CompNode::CompNode(const std::shared_ptr<node::Node>& node)
	: m_node(node)
{
}

}