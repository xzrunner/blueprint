#pragma once

#include "blueprint/Node.h"

namespace bp
{
namespace node
{

class EventBeginPlay : public Node
{
public:
	EventBeginPlay();

private:
	std::shared_ptr<Pins> m_output;

}; // EventBeginPlay

}
}