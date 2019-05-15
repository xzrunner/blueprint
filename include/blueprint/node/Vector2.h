#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

namespace bp
{
namespace node
{

class Vector2 : public Node
{
public:
    Vector2()
        : Node("Vector2")
    {
        AddPin(std::make_shared<Pin>(false, 0, PIN_FLOAT2, "Out", *this));
        Layout();
    }

    void  SetValue(const sm::vec2& val) { m_val = val; }
    auto& GetValue() const { return m_val; }

private:
    sm::vec2 m_val;

    RTTR_ENABLE(Node)

}; // Vector2

}
}