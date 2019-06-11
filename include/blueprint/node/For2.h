#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

namespace bp
{
namespace node
{

class For2 : public Node
{
public:
    For2()
        : Node("For2")
    {
        AddPin(std::make_shared<Pin>(true, 0, PIN_ANY_VAR, "In", *this));

        AddPin(std::make_shared<Pin>(false, ID_OUT,     PIN_ANY_VAR, "Out",    *this));
        AddPin(std::make_shared<Pin>(false, ID_INDEX_0, PIN_INTEGER, "Index0", *this));
        AddPin(std::make_shared<Pin>(false, ID_INDEX_1, PIN_INTEGER, "Index1", *this));

        Layout();
    }

    int i_begin = 0;
    int i_end   = 0;
    int i_step  = 0;

    int j_begin = 0;
    int j_end   = 0;
    int j_step  = 0;

    int i_curr = 0;
    int j_curr = 0;

    enum OutputID
    {
        ID_OUT = 0,
        ID_INDEX_0,
        ID_INDEX_1
    };

    RTTR_ENABLE(Node)

}; // For2

}
}