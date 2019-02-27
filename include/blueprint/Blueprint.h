#pragma once

#include "blueprint/typedef.h"

#include <cu/cu_macro.h>

#include <vector>
#include <memory>

namespace bp
{

class Blueprint
{
public:
    auto& GetAllNodes() { return m_nodes; }

private:
    void Init();
    void InitNodes();

private:
    std::vector<NodePtr> m_nodes;
    
    CU_SINGLETON_DECLARATION(Blueprint)


}; // Blueprint

}