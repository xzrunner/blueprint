#include "blueprint/node/Function.h"

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
}

}
}