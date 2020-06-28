#pragma once

#include "blueprint/node/SubGraph.h"

namespace bp
{
namespace node
{

template <typename T>
SubGraph<T>::SubGraph(const std::string& title, const std::string& back_name, const std::string& front_name,
                      const std::function<void(const bp::Node&, dag::Node<T>&)>& front2back)
    : Node(title)
	, m_back_name(back_name)
	, m_front_name(front_name)
	, m_front2back(front2back)
{
    m_all_input.push_back(std::make_shared<bp::Pin>(
        true, 0, bp::PIN_ANY_VAR, "prev", *this
    ));
    m_all_output.push_back(std::make_shared<bp::Pin>(
        false, 0, bp::PIN_ANY_VAR, "next", *this
    ));
}

template <typename T>
void SubGraph<T>::SetChildren(const std::vector<bp::NodePtr>& children, bool rebuild_graph)
{
    if (m_children == children) {
        return;
    }

	PrepareGraph();

	m_children = children;

	if (rebuild_graph)
	{
		m_graph->OnClearAllNodes();
		for (auto& c : children) {
			m_graph->OnAddNode(*c);
		}

		m_graph->OnRebuildConnection();
	}
}

template <typename T>
void SubGraph<T>::AddChild(const bp::NodePtr& child)
{
	PrepareGraph();

    m_children.push_back(child);

    m_graph->OnAddNode(*child);
}

template <typename T>
void SubGraph<T>::RemoveChild(const bp::NodePtr& child)
{
	PrepareGraph();

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
	PrepareGraph();

    m_children.clear();

    m_graph->OnClearAllNodes();
}

template <typename T>
std::shared_ptr<dag::Graph<T>>
SubGraph<T>::GetBackGraph() const
{
    return m_graph ? m_graph->GetEval() : nullptr;
}

template <typename T>
void SubGraph<T>::PrepareGraph()
{
	if (!m_graph) {
		m_graph = std::make_shared<BackendGraph<T>>(
			m_back_name, m_front_name, m_front2back
		);
	}
}

}
}