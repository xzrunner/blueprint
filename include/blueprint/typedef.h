#pragma once

#include <memory>

namespace bp
{

class Node;
using NodePtr = std::shared_ptr<Node>;

static const char* PAGE_TYPE = "blueprint";

}