#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class GetReference : public Node
{
public:
    GetReference()
        : Node("GetReference")
    {
        AddPins(std::make_shared<Pins>(false, 0, PINS_ANY_VAR, "Out", *this));

        Layout();
    }

    void SetName(const std::string& name)
    {
        m_name = name;
        m_title = "Get " + name;
        SetGetVarNameChanged(true);
    }
    auto& GetName() const { return m_name; }

private:
    std::string m_name;

    RTTR_ENABLE(Node)

}; // GetReference

}
}