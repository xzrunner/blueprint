#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class SetLocalVar : public Node
{
public:
    SetLocalVar()
        : Node("SetLocalVar")
    {
        AddPins(std::make_shared<Pins>(true, 0, PINS_PORT, "In", *this));
        AddPins(std::make_shared<Pins>(false, 0, PINS_PORT, "Out", *this));

        Layout();
    }

    void  SetVarName(const std::string& name) { m_var_name = name; }
    auto& GetVarName() const { return m_var_name; }

private:
    std::string m_var_name;

    RTTR_ENABLE(Node)

}; // SetLocalVar

}
}