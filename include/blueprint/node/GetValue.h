#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class GetValue : public Node
{
public:
    GetValue()
        : Node("GetValue")
    {
        AddPins(std::make_shared<Pins>(false, 0, PINS_ANY_VAR, "Out", *this));

        Layout();
    }

    void SetName(const std::string& name) { 
        m_name = name; 
        m_title = "Get " + name;
    }
    auto& GetName() const { return m_name; }

private:
    std::string m_name;

    RTTR_ENABLE(Node)

}; // GetValue

}
}