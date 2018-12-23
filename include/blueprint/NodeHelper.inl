#pragma once

#include "blueprint/NodeHelper.h"
#include "blueprint/Connecting.h"
#include "blueprint/Pins.h"

#include <queue>

namespace bp
{

template <typename T>
bool NodeHelper::HasInputNode(const Node& node)
{
	auto& src_inputs = node.GetAllInput();
	std::queue<std::shared_ptr<Pins>> inputs;
	for (auto& src : src_inputs) {
		inputs.push(src);
	}
	while (!inputs.empty())
	{
		auto input = inputs.front(); inputs.pop();
		auto& conns = input->GetConnecting();
		if (conns.empty()) {
			continue;
		}
		assert(conns.size() == 1);

		auto& node = conns[0]->GetFrom()->GetParent();
		if (node.get_type() == rttr::type::get<T>()) {
			return true;
		}

		for (auto& from : node.GetAllInput()) {
			inputs.push(from);
		}
	}

	return false;
}

}