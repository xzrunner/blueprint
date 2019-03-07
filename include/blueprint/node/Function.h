#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class Function : public Node
{
public:
    Function();

    auto& GetName() const { return m_name; }
    void  SetName(const std::string& name);

    auto& GetFilepath() const { return m_filepath; }
    void  SetFilepath(const std::string& filepath);

private:
    std::string m_name;

    std::string m_filepath;

    RTTR_ENABLE(Node)

}; // Function

}
}