#include "blueprint/node/SubGraph.h"

namespace bp
{
namespace node
{

SubGraph::SubGraph(const std::string& title)
    : Node(title)
{
}


void SubGraph::AddChild(const bp::NodePtr& child)
{
    m_children.push_back(child);
}

void SubGraph::RemoveChild(const bp::NodePtr& child)
{
    for (auto itr = m_children.begin(); itr != m_children.end(); )
    {
        if (*itr == child) {
            itr = m_children.erase(itr);
        } else {
            ++itr;
        }
    }
}

void SubGraph::ClearAllChildren()
{
    m_children.clear();
}

}
}