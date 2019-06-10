#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"
#include "blueprint/ReflectPropTypes.h"

namespace bp
{
namespace node
{

class Script : public Node
{
public:
    Script()
        : Node("Script")
    {
        AddPin(std::make_shared<Pin>(true, 0, PIN_INTEGER, "For Index", *this));

        AddPin(std::make_shared<Pin>(false, 0, PIN_ANY_VAR, "Out", *this));

        Layout();
    }

    auto& GetText() const { return m_text; }
    void  SetText(const std::string& text) { m_text = text; }

    auto GetRetType() const { return m_ret_type; }
    void SetRetType(VariantType type) { m_ret_type = type; }

private:
    std::string m_text;

    VariantType m_ret_type = VariantType::Void;

    RTTR_ENABLE(Node)

}; // Script

}
}