#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class Output : public Node
{
public:
    Output();

    auto& GetName() const { return m_name; }
    void  SetName(const std::string& name);

    int  GetType() const { return m_type; }
    void SetType(int type) { m_type = type; }

    static const char* STR_TYPE;

private:
    std::string m_name;

    int m_type = -1;

    RTTR_ENABLE(Node)

}; // Output

}
}
