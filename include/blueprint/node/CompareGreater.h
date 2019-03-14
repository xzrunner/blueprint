#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class CompareGreater : public Node
{
public:
    CompareGreater()
        : Node("Compare(A>B)")
    {
        AddPins(std::make_shared<Pins>(true, 0, PINS_ANY_VAR, "A",     *this));
        AddPins(std::make_shared<Pins>(true, 1, PINS_ANY_VAR, "B",     *this));
        AddPins(std::make_shared<Pins>(true, 2, PINS_ANY_VAR, "True",  *this));
        AddPins(std::make_shared<Pins>(true, 3, PINS_ANY_VAR, "False", *this));

        AddPins(std::make_shared<Pins>(false, 0, PINS_ANY_VAR, "Out",  *this));

        Layout();
    }

    enum InputID
    {
        ID_A = 0,
        ID_B,
        ID_TRUE,
        ID_FALSE,
    };

    RTTR_ENABLE(Node)

}; // CompareGreater

}
}