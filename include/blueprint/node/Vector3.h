#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

namespace bp
{
namespace node
{

class Vector3 : public Node
{
public:
    Vector3()
        : Node("Vector3")
    {
        AddPin(std::make_shared<Pin>(false, 0, PIN_FLOAT3, "Out", *this));
        Layout();
    }

    void  SetValue(const sm::vec3& val) { m_val = val; }
    auto& GetValue() const { return m_val; }

private:
    sm::vec3 m_val;

    RTTR_ENABLE(Node)

}; // Vector3

}
}