#include "blueprint/node/Function.h"
#include "blueprint/node/Input.h"
#include "blueprint/node/Output.h"

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

void Function::AddInputPort(const std::shared_ptr<Input>& input)
{
    AddNode<Input>(m_input_nodes, m_all_input, input, true);
}

void Function::RemoveInputPort(const std::shared_ptr<Input>& input)
{
    RemoveNode<Input>(m_input_nodes, m_all_input, input);
}

void Function::AddOutputPort(const std::shared_ptr<Output>& output)
{
    AddNode<Output>(m_output_nodes, m_all_output, output, false);
}

void Function::RemoveOutputPort(const std::shared_ptr<Output>& output)
{
    RemoveNode<Output>(m_output_nodes, m_all_output, output);
}

void Function::ClearAllPorts()
{
    for (auto& i : m_input_nodes) {
        i->SetParent(nullptr);
    }
    for (auto& o : m_output_nodes) {
        o->SetParent(nullptr);
    }

    m_input_nodes.clear();
    m_output_nodes.clear();
    m_all_input.clear();
    m_all_output.clear();
}

void Function::UpdatePins()
{
    assert(m_input_nodes.size() == m_all_input.size());
    for (int i = 0, n = m_input_nodes.size(); i < n; ++i) {
        m_all_input[i]->SetName(m_input_nodes[i]->GetName());
    }

    assert(m_output_nodes.size() == m_all_output.size());
    for (int i = 0, n = m_output_nodes.size(); i < n; ++i) {
        m_all_output[i]->SetName(m_output_nodes[i]->GetName());
    }
}

}
}