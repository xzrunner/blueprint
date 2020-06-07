#pragma once

#include "blueprint/Node.h"

namespace bp
{
namespace node
{

class SubGraph : public Node
{
public:
    SubGraph(const std::string& title);

    auto& GetAllChildren() const { return m_children; }

    void AddChild(const bp::NodePtr& child);
    void RemoveChild(const bp::NodePtr& child);
    void ClearAllChildren();

private:
    std::vector<bp::NodePtr> m_children;

    RTTR_ENABLE(Node)

}; // SubGraph

}
}