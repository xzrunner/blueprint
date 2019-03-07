#include "blueprint/node/Output.h"
#include "blueprint/node/Function.h"

namespace bp
{
namespace node
{

const char* Output::STR_TYPE = "Type";

Output::Output()
    : Node("Output")
{
    AddPins(std::make_shared<Pins>(true, 0, PINS_ANY_VAR, "Out", *this));

    Layout();
}

void Output::SetName(const std::string& name)
{
    m_name  = name;
    m_title = name;

    auto p = m_parent.lock();
    if (p) {
        p->UpdatePins();
    }
}

}
}