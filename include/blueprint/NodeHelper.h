#pragma once

#include "blueprint/typedef.h"

#include <SM_Matrix.h>
#include <SM_Matrix2D.h>
#include <node0/typedef.h>

#include <vector>

namespace bp
{

class Node;
class Pins;

class NodeHelper
{
public:
	template <typename T>
	static bool HasInputNode(const Node& node);

    static void RemoveDefaultNode(const bp::Pins& p);

    static void LoadFunctionNode(const n0::SceneNodePtr& obj, const bp::NodePtr& node);

    static void LoadConnections(const std::vector<n0::SceneNodePtr>& nodes,
        const std::string& filepath);

}; // NodeHelper

}

#include "blueprint/NodeHelper.inl"