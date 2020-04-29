#pragma once

#include "blueprint/typedef.h"

#include <SM_Matrix.h>
#include <SM_Matrix2D.h>
#include <node0/typedef.h>

#include <vector>

namespace ur { class Device; }

namespace bp
{

class Node;
class Pin;

class NodeHelper
{
public:
	template <typename T>
	static bool HasInputNode(const Node& node);

    static void RemoveDefaultNode(const bp::Pin& p);

    static void LoadFunctionNode(const ur::Device& dev,
        const n0::SceneNodePtr& obj, const bp::NodePtr& node);

    static sm::Matrix2D CalcPreviewMat(const Node& node, const sm::Matrix2D& mt);

}; // NodeHelper

}

#include "blueprint/NodeHelper.inl"