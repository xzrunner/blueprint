#pragma once

#include "blueprint/NodeLayout.h"
#include "blueprint/Pin.h"

#include <dag/Node.h>

namespace bp
{

template<typename T>
BackendAdapter<T>::BackendAdapter(const std::string& back_name,
                                  std::function<PinDesc(const typename dag::Node<T>::Port&)> back2front)
    : m_back_name(back_name)
    , m_pin_back2front(back2front)
{
}

template<typename T>
void BackendAdapter<T>::InitNodePins(Node& node, const std::vector<PinDesc>& input, const std::vector<PinDesc>& output)
{
    InitPinsImpl(node, true, input);
    InitPinsImpl(node, false, output);
    NodeLayout::UpdateNodeStyle(node);
}

template<typename T>
void BackendAdapter<T>::InitNodePins(Node& node, const std::string& name)
{
	rttr::type t = rttr::type::get_by_name(m_back_name + "::" + name);
    if (!t.is_valid()) {
        return;
    }

	rttr::variant var = t.create();
	assert(var.is_valid());

	auto method_imports = t.get_method("GetImports");
	assert(method_imports.is_valid());
	auto var_imports = method_imports.invoke(var);
	assert(var_imports.is_valid()
		&& var_imports.is_type<std::vector<dag::Node<T>::Port>>());
	auto& imports = var_imports.get_value<std::vector<dag::Node<T>::Port>>();

	auto method_exports = t.get_method("GetExports");
	assert(method_exports.is_valid());
	auto var_exports = method_exports.invoke(var);
	assert(var_exports.is_valid()
		&& var_exports.is_type<std::vector<dag::Node<T>::Port>>());
	auto& exports = var_exports.get_value<std::vector<dag::Node<T>::Port>>();

	std::vector<PinDesc> input(imports.size()), output(exports.size());
    for (size_t i = 0, n = imports.size(); i < n; ++i) {
        input[i] = m_pin_back2front(imports[i]);
    }
    for (size_t i = 0, n = exports.size(); i < n; ++i) {
        output[i] = m_pin_back2front(exports[i]);
    }

    InitNodePins(node, input, output);
}

template<typename T>
void BackendAdapter<T>::InitNodePins(Node& dst, const dag::Node<T>& src)
{
    auto& imports = src.GetImports();
    auto& exports = src.GetExports();

    std::vector<PinDesc> input(imports.size()), output(exports.size());
    for (size_t i = 0, n = imports.size(); i < n; ++i) {
        input[i] = m_pin_back2front(imports[i]);
    }
    for (size_t i = 0, n = exports.size(); i < n; ++i) {
        output[i] = m_pin_back2front(exports[i]);
    }

    InitNodePins(dst, input, output);
}

template<typename T>
void BackendAdapter<T>::InitPinsImpl(Node& node, bool is_input, const std::vector<PinDesc>& pins)
{
    auto& dst = is_input ?
        const_cast<std::vector<std::shared_ptr<Pin>>&>(node.GetAllInput()) :
        const_cast<std::vector<std::shared_ptr<Pin>>&>(node.GetAllOutput());
    auto old_pins = dst;

	dst.clear();
	dst.reserve(pins.size());
	for (auto& d : pins)
	{
        std::shared_ptr<bp::Pin> p = nullptr;
        for (auto& old_p : old_pins) {
            if (old_p->GetName() == d.name &&
                old_p->GetType() == d.type) {
                p = old_p;
                break;
            }
        }
        if (!p) {
            p = std::make_shared<bp::Pin>(is_input, dst.size(), d.type, d.name, node);
        }
		if (!Node::CheckPinName(*p, dst)) {
			assert(0);
			return;
		}
		dst.push_back(p);
	}
}

}