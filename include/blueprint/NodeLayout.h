#pragma once

#include <blueprint/Node.h>

#include <cstdint>

namespace bp
{

class Node;

class NodeLayout
{
public:
	static const uint32_t DEFAULT_WIDTH;
	static const uint32_t DEFAULT_HEIGHT;

	static const uint32_t TITLE_HEIGHT;

	static const float PIN_RADIUS;

	static const float CONNECTING_BEZIER_DIST;

public:
	static void UpdateNodeStyle(Node& node);

	static sm::vec2 GetPinPos(const Pin& pin);
	static sm::vec2 GetPinPos(const Node& node, bool is_input, size_t idx);

}; // NodeLayout

}