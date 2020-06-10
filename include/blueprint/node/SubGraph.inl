#pragma once

#include "blueprint/node/SubGraph.h"

namespace bp
{
namespace node
{

template <typename T>
SubGraph<T>::SubGraph(const std::string& title,
                      const std::string& back_name,
                      const std::string& front_name)
    : Node(title)
{
    m_graph = std::make_shared<BackendGraph<T>>(back_name, front_name);
}

template <typename T>
void SubGraph<T>::SetChildren(const std::vector<bp::NodePtr>& children)
{
    if (m_children == children) {
        return;
    }

    m_graph->OnClearAllNodes();
    for (auto& c : children) {
        m_graph->OnAddNode(*c);
    }

    m_graph->OnRebuildConnection();
}

template <typename T>
void SubGraph<T>::AddChild(const bp::NodePtr& child)
{
    m_children.push_back(child);

    m_graph->OnAddNode(*child);
}

template <typename T>
void SubGraph<T>::RemoveChild(const bp::NodePtr& child)
{
    m_graph->OnRemoveNode(*child);

    for (auto itr = m_children.begin(); itr != m_children.end(); )
    {
        if (*itr == child) {
            itr = m_children.erase(itr);
        } else {
            ++itr;
        }
    }
}

template <typename T>
void SubGraph<T>::ClearAllChildren()
{
    m_children.clear();

    m_graph->OnClearAllNodes();
}

template <typename T>
std::shared_ptr<dag::Graph<T>>
SubGraph<T>::GetBackGraph() const
{
    return m_graph->GetEval();
}

}
}