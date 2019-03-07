#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class Input;
class Output;

class Function : public Node
{
public:
    Function();

    auto& GetName() const { return m_name; }
    void  SetName(const std::string& name);

    auto& GetFilepath() const { return m_filepath; }
    void  SetFilepath(const std::string& filepath);

    void AddInputPort(const std::shared_ptr<Input>& input);
    void RemoveInputPort(const std::shared_ptr<Input>& input);
    void AddOutputPort(const std::shared_ptr<Output>& output);
    void RemoveOutputPort(const std::shared_ptr<Output>& output);
    void ClearAllPorts();

    void UpdatePins();

private:
    template<typename NodeType>
    void AddNode(std::vector<std::shared_ptr<NodeType>>& node_list, std::vector<std::shared_ptr<Pins>>& pins_list,
        const std::shared_ptr<NodeType>& node, bool is_input);

    template<typename NodeType>
    void RemoveNode(std::vector<std::shared_ptr<NodeType>>& node_list,
        std::vector<std::shared_ptr<Pins>>& pins_list, const std::shared_ptr<NodeType>& node);

private:
    std::string m_name;

    std::string m_filepath;

    std::vector<std::shared_ptr<Input>>  m_input_nodes;
    std::vector<std::shared_ptr<Output>> m_output_nodes;

    RTTR_ENABLE(Node)

}; // Function

}
}

#include "blueprint/node/Function.inl"