#pragma once

#include <SM_Vector.h>

namespace bp
{

class Connecting;

class Evaluator
{
public:
    static float CalcFloat(const bp::Connecting& conn, int for_idx = 0);
    static sm::vec3 CalcFloat3(const bp::Connecting& conn, int for_idx = 0);

}; // Evaluator

}