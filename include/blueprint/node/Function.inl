#pragma once

namespace bp
{
namespace node
{

template<typename NodeType>
inline void Function::AddNode(std::vector<std::shared_ptr<NodeType>>& node_list,
                              std::vector<std::shared_ptr<Pin>>& pin_list,
                              const std::shared_ptr<NodeType>& node, bool is_input, bool need_layout)
{
    node_list.push_back(node);

    int type = PIN_ANY_VAR;        // todo
    auto pin = std::make_shared<Pin>(is_input, pin_list.size(), type, node->GetName(), *this);
    pin_list.push_back(pin);

    if (need_layout) {
        Layout();
    }
}

template<typename NodeType>
inline void Function::RemoveNode(std::vector<std::shared_ptr<NodeType>>& node_list,
                                 std::vector<std::shared_ptr<Pin>>& pin_list,
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
        pin_list[i] = pin_list[i + 1];
        pin_list[i]->SetPosIdx(pin_list[i]->GetPosIdx());
    }
    pin_list.pop_back();

    node_list.erase(node_list.begin() + i);

    Layout();
}

}
}