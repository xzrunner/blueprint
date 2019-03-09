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
    if (m_name == name) {
        return;
    }

    m_name  = name;
    m_title = name;

    auto p = m_parent.lock();
    if (p) {
        p->UpdatePins();
    }
}

void Input::SetType(int type)
{
    if (m_type == type) {
        return;
    }

    m_type = type;

    m_all_output[0]->SetType(type);

    auto p = m_parent.lock();
    if (p) {
        p->UpdatePins();
    }
}

}
}