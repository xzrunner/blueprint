#pragma once

#include "blueprint/typedef.h"

#include <SM_Vector.h>

#include <vector>

namespace bp
{

class Connecting;

class Evaluator
{
public:
    static float CalcFloat(const bp::Connecting& conn);
    static sm::vec3 CalcFloat3(const bp::Connecting& conn);

    static void TopologicalSorting(std::vector<NodePtr>& nodes);

}; // Evaluator

}