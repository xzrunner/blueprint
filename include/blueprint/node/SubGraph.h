#pragma once

#include "blueprint/Node.h"
#include "blueprint/BackendGraph.h"

namespace bp
{
namespace node
{

template<typename T>
class SubGraph : public Node
{
public:
    SubGraph(const std::string& title,
        const std::string& back_name, const std::string& front_name);

    auto& GetChildren() const { return m_children; }
    void SetChildren(const std::vector<bp::NodePtr>& children);

    void AddChild(const bp::NodePtr& child);
    void RemoveChild(const bp::NodePtr& child);
    void ClearAllChildren();

    std::shared_ptr<dag::Graph<T>> GetBackGraph() const;

protected:
    std::vector<bp::NodePtr> m_children;

    std::shared_ptr<BackendGraph<T>> m_graph = nullptr;

    RTTR_ENABLE(Node)

}; // SubGraph

}
}

#include "blueprint/node/SubGraph.inl"