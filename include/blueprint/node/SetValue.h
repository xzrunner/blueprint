#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

namespace bp
{
namespace node
{

class SetValue : public Node
{
public:
    SetValue()
        : Node("SetValue")
    {
        AddPin(std::make_shared<Pin>(true, 0, PIN_ANY_VAR, "In", *this));
        AddPin(std::make_shared<Pin>(false, 0, PIN_ANY_VAR, "Out", *this));

        Layout();
    }

    void SetName(const std::string& name)
    {
        m_name = name;
        m_title = "Set " + name;
        SetGetVarNameChanged(true);
    }
    auto& GetName() const { return m_name; }

private:
    std::string m_name;

    RTTR_ENABLE(Node)

}; // SetValue

}
}