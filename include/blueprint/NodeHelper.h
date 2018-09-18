#pragma once

#include <SM_Matrix.h>
#include <SM_Matrix2D.h>

namespace bp
{

class Node;

class NodeHelper
{
public:
	template <typename T>
	static bool HasInputNode(const Node& node);

}; // NodeHelper

}

#include "blueprint/NodeHelper.inl"