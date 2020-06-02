#pragma once

#include "blueprint/typedef.h"

#include <dag/Node.h>
#include <node0/typedef.h>

#include <vector>

namespace ur { class Device; }

namespace bp
{

class SerializeHelper
{
public:
    template <typename T>
    static void SetupNodes(const std::vector<n0::SceneNodePtr>& nodes, std::vector<bp::NodePtr>& front_nodes,
        std::vector<std::shared_ptr<dag::Node<T>>>& back_nodes);

    template <typename T>
    static void SetupConnections(const std::string& filepath, const std::vector<n0::SceneNodePtr>& nodes,
        std::vector<bp::NodePtr>& front_nodes, std::vector<std::shared_ptr<dag::Node<T>>>& back_nodes);

}; // SerializeHelper

}

#include "blueprint/SerializeHelper.inl"