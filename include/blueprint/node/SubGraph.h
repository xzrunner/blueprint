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
    SubGraph(const std::string& title, const std::string& back_name, const std::string& front_name,
        const std::function<void(const bp::Node&, dag::Node<T>&)>& front2back);

    auto& GetChildren() const { return m_children; }
    void SetChildren(const std::vector<bp::NodePtr>& children, bool rebuild_graph);

    void AddChild(const bp::NodePtr& child);
    void RemoveChild(const bp::NodePtr& child);
    void ClearAllChildren();

    auto GetGraph() const { return m_graph; }

    std::shared_ptr<dag::Graph<T>> GetBackGraph() const;

protected:
	void PrepareGraph();

protected:
    std::vector<bp::NodePtr> m_children;

    mutable std::shared_ptr<BackendGraph<T>> m_graph = nullptr;

private:
	std::string m_back_name, m_front_name;
	std::function<void(const bp::Node&, dag::Node<T>&)> m_front2back = nullptr;

    RTTR_ENABLE(Node)

}; // SubGraph

}
}

#include "blueprint/node/SubGraph.inl"