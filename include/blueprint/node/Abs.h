#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class Abs : public Node
{
public:
    Abs()
        : Node("Abs")
    {
        AddPins(std::make_shared<Pins>(true, 0, PINS_ANY_VAR, "In", *this));
        AddPins(std::make_shared<Pins>(false, 0, PINS_ANY_VAR, "Out", *this));
        Layout();
    }

    RTTR_ENABLE(Node)

}; // Abs

}
}