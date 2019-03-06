#include "blueprint/node/Output.h"

namespace bp
{
namespace node
{

const char* Output::STR_TYPE = "Type";

Output::Output()
    : Node("Output")
{
    AddPins(std::make_shared<Pins>(true, 0, PINS_ANY_VAR, "Out", *this));

    Layout();
}

}
}