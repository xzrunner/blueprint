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

    auto& GetFilepath() const { return m_filepath; }
    void  SetFilepath(const std::string& filepath);

private:
    std::string m_filepath;

    RTTR_ENABLE(Node)

}; // Function

}
}
