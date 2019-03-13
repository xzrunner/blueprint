#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class Proxy : public Node
{
public:
    Proxy()
        : Node("Proxy")
    {
        Layout();
    }

    auto GetReal() const { return m_real; }
    void SetReal(const std::shared_ptr<Pins>& real) { m_real = real; }

    void ResizeOutputs(int num)
    {
        m_all_output.clear();
        for (int i = 0; i < num; ++i) {
            AddPins(std::make_shared<bp::Pins>(false, i, PINS_ANY_VAR, std::to_string(i), *this));
        }
    }

private:
    std::shared_ptr<Pins> m_real = nullptr;

    RTTR_ENABLE(Node)

}; // Proxy

}
}