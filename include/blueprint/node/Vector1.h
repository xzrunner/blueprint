#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

namespace bp
{
namespace node
{

class Vector1 : public Node
{
public:
    Vector1()
        : Node("Vector1")
    {
        AddPin(std::make_shared<Pin>(false, 0, PIN_FLOAT, "Out", *this));
        Layout();
    }

    void  SetValue(float val) { m_val = val; }
    float GetValue() const { return m_val; }

private:
    float m_val = 0;

    RTTR_ENABLE(Node)

}; // Vector1

}
}