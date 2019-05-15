#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

namespace bp
{
namespace node
{

class Vector4 : public Node
{
public:
    Vector4()
        : Node("Vector4")
    {
        AddPin(std::make_shared<Pin>(false, 0, PIN_FLOAT4, "Out", *this));
        Layout();
    }

    void  SetValue(const sm::vec4& val) { m_val = val; }
    auto& GetValue() const { return m_val; }

private:
    sm::vec4 m_val;

    RTTR_ENABLE(Node)

}; // Vector4

}
}