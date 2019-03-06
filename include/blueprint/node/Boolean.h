#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class Boolean : public Node
{
public:
    Boolean()
        : Node("Boolean")
    {
        AddPins(std::make_shared<Pins>(false, 0, PINS_BOOLEAN, "Out", *this));

        Layout();
    }

    auto GetValue() const { return m_val; }
    void SetValue(bool val) { m_val = val; }

private:
    bool m_val = false;

    RTTR_ENABLE(Node)

}; // Boolean

}
}