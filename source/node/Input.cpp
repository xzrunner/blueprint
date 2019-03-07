#include "blueprint/node/Input.h"
#include "blueprint/node/Function.h"

namespace bp
{
namespace node
{

const char* Input::STR_TYPE = "Type";

Input::Input()
    : Node("Input")
{
    AddPins(std::make_shared<Pins>(false, 0, PINS_ANY_VAR, "Out", *this));

    Layout();
}

void Input::SetName(const std::string& name)
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