#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class Switch : public Node
{
public:
    Switch()
        : Node("Switch")
        , m_toggle(true)
    {
        AddPins(std::make_shared<Pins>(true,  0, PINS_ANY_VAR, "True", *this));
        AddPins(std::make_shared<Pins>(true,  1, PINS_ANY_VAR, "False", *this));
        AddPins(std::make_shared<Pins>(false, 0, PINS_ANY_VAR, "Out", *this));

        Layout();
    }

    bool GetToggle() const { return m_toggle; }
    void SetToggle(bool toggle) { m_toggle = toggle; }

    enum InputID
    {
        ID_TRUE = 0,
        ID_FALSE
    };

private:
    bool m_toggle;

    RTTR_ENABLE(Node)

}; // Switch

}
}