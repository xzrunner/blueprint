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

void Function::SetFilepath(const std::string& filepath)
{
    m_filepath = filepath;
}

}
}