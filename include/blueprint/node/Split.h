#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

namespace bp
{
namespace node
{

class Split : public Node
{
public:
    Split()
        : Node("Split")
    {
        AddPin(std::make_shared<Pin>(true, 0, PIN_ANY_VAR, "In", *this));

        AddPin(std::make_shared<Pin>(false, ID_R, PIN_FLOAT1, "R", *this));
        AddPin(std::make_shared<Pin>(false, ID_G, PIN_FLOAT1, "G", *this));
        AddPin(std::make_shared<Pin>(false, ID_B, PIN_FLOAT1, "B", *this));
        AddPin(std::make_shared<Pin>(false, ID_A, PIN_FLOAT1, "A", *this));

        Layout();
    }

    enum OutputID
    {
        ID_R = 0,
        ID_G,
        ID_B,
        ID_A,
    };

    RTTR_ENABLE(Node)

}; // Split

}
}