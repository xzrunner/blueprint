#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

namespace bp
{
namespace node
{

class Divide : public Node
{
public:
    Divide()
        : Node("Divide")
    {
        AddPin(std::make_shared<Pin>(true, ID_A, PIN_ANY_VAR, "A", *this));
        AddPin(std::make_shared<Pin>(true, ID_B, PIN_ANY_VAR, "B", *this));

        AddPin(std::make_shared<Pin>(false, 0, PIN_ANY_VAR, "Out", *this));

        Layout();
    }

    enum InputID
    {
        ID_A = 0,
        ID_B,
    };

    RTTR_ENABLE(Node)

}; // Divide

}
}