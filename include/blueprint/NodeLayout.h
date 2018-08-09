#pragma once

#include <blueprint/Node.h>

#include <cstdint>

namespace bp
{

namespace node { class Node; }

class NodeLayout
{
public:
	static const uint32_t DEFAULT_WIDTH;
	static const uint32_t DEFAULT_HEIGHT;

	static const uint32_t TITLE_HEIGHT;

	static const float PINS_RADIUS;

public:
	static void UpdateNodeStyle(node::Node& node);

}; // NodeLayout

}