#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

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
        AddPin(std::make_shared<Pin>(true, 0, PIN_ANY_VAR, "True", *this));
        AddPin(std::make_shared<Pin>(true, 1, PIN_ANY_VAR, "False", *this));
        AddPin(std::make_shared<Pin>(true, 2, PIN_BOOLEAN, "Value", *this));

        AddPin(std::make_shared<Pin>(false, 0, PIN_ANY_VAR, "Out", *this));

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