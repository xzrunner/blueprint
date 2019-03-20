#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class Hub : public Node
{
public:
    Hub()
        : Node("Hub")
    {
        AddPins(std::make_shared<Pins>(true, 0, PINS_ANY_VAR, "A", *this));
        AddPins(std::make_shared<Pins>(true, 1, PINS_ANY_VAR, "B", *this));

        AddPins(std::make_shared<Pins>(false, 0, PINS_ANY_VAR, "Out", *this));

        Layout();
    }

    RTTR_ENABLE(Node)

}; // Hub

}
}