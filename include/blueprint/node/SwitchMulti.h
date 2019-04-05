#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

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
        AddPin(std::make_shared<Pin>(true, 0, PIN_ANY_VAR, "A", *this));
        AddPin(std::make_shared<Pin>(true, 1, PIN_ANY_VAR, "B", *this));
        AddPin(std::make_shared<Pin>(true, 2, PIN_ANY_VAR, "C", *this));
        AddPin(std::make_shared<Pin>(true, 3, PIN_ANY_VAR, "D", *this));
        AddPin(std::make_shared<Pin>(true, 4, PIN_ANY_VAR, "E", *this));
        AddPin(std::make_shared<Pin>(true, 5, PIN_ANY_VAR, "F", *this));
        AddPin(std::make_shared<Pin>(true, 6, PIN_ANY_VAR, "G", *this));
        AddPin(std::make_shared<Pin>(true, 7, PIN_ANY_VAR, "H", *this));

        AddPin(std::make_shared<Pin>(false, 0, PIN_ANY_VAR, "Out", *this));

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