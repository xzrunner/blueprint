#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

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
        AddPin(std::make_shared<Pin>(false, 0, PIN_BOOLEAN, "Out", *this));

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