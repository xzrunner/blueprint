#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

namespace bp
{
namespace node
{

class Function;

class Input : public Node
{
public:
    Input();

    auto& GetName() const { return m_name; }
    void  SetName(const std::string& name);

    int  GetType() const { return m_type; }
    void SetType(int type);

    void SetParent(const std::shared_ptr<Function>& func) {
        m_parent = func;
    }
    auto GetParent() const { return m_parent.lock(); }

    static const char* STR_TYPE;

private:
    std::string m_name;

    int m_type = -1;

    std::weak_ptr<Function> m_parent;

    RTTR_ENABLE(Node)

}; // Input

}
}
