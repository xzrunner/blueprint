#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class Input : public Node
{
public:
    Input();

    int  GetType() const { return m_type; }
    void SetType(int type) { m_type = type; }

    static const char* STR_TYPE;

private:
    int m_type = -1;

    RTTR_ENABLE(Node)

}; // Input

}
}
