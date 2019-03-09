#include "blueprint/node/Function.h"
#include "blueprint/node/Input.h"
#include "blueprint/node/Output.h"
#include "blueprint/CompNode.h"

#include <node0/SceneNode.h>

#include <boost/filesystem.hpp>

namespace bp
{
namespace node
{

Function::Function()
    : Node("Function")
{
    Layout();
}

void Function::SetName(const std::string& name)
{
    if (!name.empty()) {
        m_name = name;
        m_title = m_name;
    }
}

void Function::SetFilepath(const std::string& filepath)
{
    m_filepath = filepath;
    SetName(boost::filesystem::path(filepath).stem().string());
}

bool Function::AddChild(std::shared_ptr<Function>& parent, const n0::SceneNodePtr& child)
{
    bool port_dirty = false;

    assert(child->HasUniqueComp<CompNode>());

    parent->m_children.push_back(child);

    auto& cbp = child->GetUniqueComp<CompNode>();
    auto& bp_node = cbp.GetNode();
    auto bp_type = bp_node->get_type();
    if (bp_type == rttr::type::get<node::Input>())
    {
        auto input = std::static_pointer_cast<node::Input>(bp_node);
        input->SetParent(parent);
        parent->AddNode<Input>(parent->m_input_nodes, parent->m_all_input, input, true, true);
    }
    else if (bp_type == rttr::type::get<node::Output>())
    {
        auto output = std::static_pointer_cast<node::Output>(bp_node);
        output->SetParent(parent);
        parent->AddNode<Output>(parent->m_output_nodes, parent->m_all_output, output, false, true);
    }

    return port_dirty;
}

bool Function::RemoveChild(std::shared_ptr<Function>& parent, const n0::SceneNodePtr& child)
{
    bool port_dirty = false;

    auto& children = parent->m_children;
    children.erase(std::find(children.begin(), children.end(), child));

    auto& cbp = child->GetUniqueComp<CompNode>();
    auto& bp_node = cbp.GetNode();
    auto bp_type = bp_node->get_type();
    if (bp_type == rttr::type::get<node::Input>()) {
        auto input = std::static_pointer_cast<node::Input>(bp_node);
        parent->RemoveNode<Input>(parent->m_input_nodes, parent->m_all_input, input);
    } else if (bp_type == rttr::type::get<node::Output>()) {
        auto output = std::static_pointer_cast<node::Output>(bp_node);
        parent->RemoveNode<Output>(parent->m_output_nodes, parent->m_all_output, output);
    }

    return port_dirty;
}

void Function::SetChildren(std::shared_ptr<Function>& parent, const std::vector<n0::SceneNodePtr>& children)
{
    parent->m_children = children;

    parent->m_input_nodes.clear();
    parent->m_output_nodes.clear();
    parent->m_all_input.clear();
    parent->m_all_output.clear();

    for (auto& c : children)
    {
        assert(c->HasUniqueComp<CompNode>());
        auto& cnode = c->GetUniqueComp<CompNode>();
        auto bp_node = cnode.GetNode();
        auto bp_type = bp_node->get_type();
        if (bp_type == rttr::type::get<node::Input>())
        {
            auto input = std::static_pointer_cast<node::Input>(bp_node);
            input->SetParent(parent);
            parent->AddNode<Input>(parent->m_input_nodes, parent->m_all_input, input, true, false);
        }
        else if (bp_type == rttr::type::get<node::Output>())
        {
            auto output = std::static_pointer_cast<node::Output>(bp_node);
            output->SetParent(parent);
            parent->AddNode<Output>(parent->m_output_nodes, parent->m_all_output, output, false, false);
        }
    }

    parent->UpdatePins();

    parent->Layout();
}

void Function::ClearChildren()
{
    m_children.clear();

    m_input_nodes.clear();
    m_output_nodes.clear();
    m_all_input.clear();
    m_all_output.clear();

    Layout();
}

void Function::UpdatePins()
{
    assert(m_input_nodes.size() == m_all_input.size());
    for (int i = 0, n = m_input_nodes.size(); i < n; ++i) {
        m_all_input[i]->SetName(m_input_nodes[i]->GetName());
        m_all_input[i]->SetType(m_input_nodes[i]->GetType());
    }

    assert(m_output_nodes.size() == m_all_output.size());
    for (int i = 0, n = m_output_nodes.size(); i < n; ++i) {
        m_all_output[i]->SetName(m_output_nodes[i]->GetName());
        m_all_output[i]->SetType(m_output_nodes[i]->GetType());
    }
}

}
}