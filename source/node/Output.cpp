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
    AddPin(std::make_shared<Pin>(true, 0, PIN_ANY_VAR, "Out", *this));

    Layout();
}

void Output::SetName(const std::string& name)
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

void Output::SetType(int type)
{
    if (m_type == type) {
        return;
    }

    m_type = type;

    m_all_input[0]->SetType(type);

    auto p = m_parent.lock();
    if (p) {
        p->UpdatePins();
    }
}

}
}