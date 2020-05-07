#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"
#include "blueprint/VarType.h"

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
        AddPin(std::make_shared<Pin>(true, ID_VAR_I, PIN_INTEGER, "i", *this));
        AddPin(std::make_shared<Pin>(true, ID_VAR_J, PIN_INTEGER, "j", *this));

        AddPin(std::make_shared<Pin>(false, 0, PIN_ANY_VAR, "Out", *this));

        Layout();
    }

    auto& GetText() const { return m_text; }
    void  SetText(const std::string& text) { m_text = text; }

    auto GetRetType() const { return m_ret_type; }
    void SetRetType(VarType type) { m_ret_type = type; }

    enum InputID
    {
        ID_VAR_I = 0,
        ID_VAR_J
    };

private:
    std::string m_text;

    VarType m_ret_type = VarType::Void;

    RTTR_ENABLE(Node)

}; // Script

}
}