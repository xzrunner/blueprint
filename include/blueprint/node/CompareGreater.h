#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

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
        AddPin(std::make_shared<Pin>(true, 0, PIN_ANY_VAR, "A",     *this));
        AddPin(std::make_shared<Pin>(true, 1, PIN_ANY_VAR, "B",     *this));
        AddPin(std::make_shared<Pin>(true, 2, PIN_ANY_VAR, "True",  *this));
        AddPin(std::make_shared<Pin>(true, 3, PIN_ANY_VAR, "False", *this));

        AddPin(std::make_shared<Pin>(false, 0, PIN_ANY_VAR, "Out",  *this));

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