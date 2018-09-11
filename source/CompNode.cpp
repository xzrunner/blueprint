#include "blueprint/CompNode.h"

#include <assert.h>

namespace bp
{

const char* const CompNode::TYPE_NAME = "bp_node";

std::unique_ptr<n0::NodeComp> CompNode::Clone(const n0::SceneNode& node) const
{
	auto comp = std::make_unique<CompNode>();
	// todo
	assert(0);
	return comp;
}

}