#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

namespace bp
{
namespace node
{

class For : public Node
{
public:
    For()
        : Node("For")
    {
        AddPin(std::make_shared<Pin>(true, 0, PIN_ANY_VAR, "In", *this));

        AddPin(std::make_shared<Pin>(false, ID_OUT,   PIN_ANY_VAR, "Out",   *this));
        AddPin(std::make_shared<Pin>(false, ID_INDEX, PIN_INTEGER, "Index", *this));

        Layout();
    }

    int index_begin = 0;
    int index_end   = 0;

    int index_step = 0;

    enum OutputID
    {
        ID_OUT = 0,
        ID_INDEX,
    };

    RTTR_ENABLE(Node)

}; // For

}
}