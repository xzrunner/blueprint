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
    {
        AddPins(std::make_shared<Pins>(true, 0, PINS_ANY_VAR, "True", *this));
        AddPins(std::make_shared<Pins>(true, 1, PINS_ANY_VAR, "False", *this));
        AddPins(std::make_shared<Pins>(true, 2, PINS_BOOLEAN, "Value", *this));

        AddPins(std::make_shared<Pins>(false, 0, PINS_ANY_VAR, "Out", *this));

        Layout();
    }

    enum InputID
    {
        ID_TRUE = 0,
        ID_FALSE,
        ID_VALUE,
    };

    RTTR_ENABLE(Node)

}; // Switch

}
}