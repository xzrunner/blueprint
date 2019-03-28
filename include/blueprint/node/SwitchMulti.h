#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class SwitchMulti : public Node
{
public:
    SwitchMulti()
        : Node("SwitchMulti")
    {
        AddPins(std::make_shared<Pins>(true, 0, PINS_ANY_VAR, "A", *this));
        AddPins(std::make_shared<Pins>(true, 1, PINS_ANY_VAR, "B", *this));
        AddPins(std::make_shared<Pins>(true, 2, PINS_ANY_VAR, "C", *this));
        AddPins(std::make_shared<Pins>(true, 3, PINS_ANY_VAR, "D", *this));
        AddPins(std::make_shared<Pins>(true, 4, PINS_ANY_VAR, "E", *this));
        AddPins(std::make_shared<Pins>(true, 5, PINS_ANY_VAR, "F", *this));
        AddPins(std::make_shared<Pins>(true, 6, PINS_ANY_VAR, "G", *this));
        AddPins(std::make_shared<Pins>(true, 7, PINS_ANY_VAR, "H", *this));

        AddPins(std::make_shared<Pins>(false, 0, PINS_ANY_VAR, "Out", *this));

        Layout();
    }

    enum InputID
    {
        ID_A = 0,
        ID_B,
        ID_C,
        ID_D,
        ID_E,
        ID_F,
        ID_G,
        ID_H
    };

    RTTR_ENABLE(Node)

}; // SwitchMulti

}
}