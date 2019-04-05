#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

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
        AddPin(std::make_shared<Pin>(true, 0, PIN_ANY_VAR, "A", *this));
        AddPin(std::make_shared<Pin>(true, 1, PIN_ANY_VAR, "B", *this));

        AddPin(std::make_shared<Pin>(false, 0, PIN_ANY_VAR, "Out", *this));

        SetExtensibleInputPorts(true);

        Layout();
    }

    RTTR_ENABLE(Node)

}; // Hub

}
}