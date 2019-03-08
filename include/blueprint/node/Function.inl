#pragma once

namespace bp
{
namespace node
{

template<typename NodeType>
inline void Function::AddNode(std::vector<std::shared_ptr<NodeType>>& node_list,
                              std::vector<std::shared_ptr<Pins>>& pins_list,
                              const std::shared_ptr<NodeType>& node, bool is_input, bool need_layout)
{
    node_list.push_back(node);

    int type = PINS_ANY_VAR;        // todo
    auto pins = std::make_shared<Pins>(is_input, pins_list.size(), type, node->GetName(), *this);
    pins_list.push_back(pins);

    if (need_layout) {
        Layout();
    }
}

template<typename NodeType>
inline void Function::RemoveNode(std::vector<std::shared_ptr<NodeType>>& node_list,
                                 std::vector<std::shared_ptr<Pins>>& pins_list,
                                 const std::shared_ptr<NodeType>& node)
{
    int i = 0;
    for (int n = node_list.size(); i < n; ++i) {
        if (node_list[i] == node) {
            break;
        }
    }
    if (i == node_list.size()) {
        return;
    }

    node->SetParent(nullptr);

    for (int n = node_list.size() - 1; i < n; ++i) {
        pins_list[i] = pins_list[i + 1];
        pins_list[i]->SetPosIdx(pins_list[i]->GetPosIdx());
    }
    pins_list.pop_back();

    node_list.erase(node_list.begin() + i);

    Layout();
}

}
}