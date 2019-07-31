#pragma once

#include "blueprint/typedef.h"

#include <set>

#include <rttr/type.h>

namespace bp
{

class TreeHelper
{
public:
    static void GetSuccessorNodes(const Node& root, std::set<const Node*>& result);
    static void GetPrecursorNodes(const Node& root, std::set<const Node*>& result);

}; // TreeHelper

}