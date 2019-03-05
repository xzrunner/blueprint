#include "blueprint/node/Input.h"

namespace bp
{
namespace node
{

const char* Input::STR_TYPE = "Type";

Input::Input()
    : Node("Input")
{
    AddPins(std::make_shared<Pins>(false, 0, PINS_ANY_VAR, "Out", *this));

    Layout();
}

}
}