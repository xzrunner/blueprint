#pragma once

#include <dag/Node.h>

#include <string>
#include <functional>

namespace bp
{

class Node;

struct PinDesc
{
    int         type;
    std::string name;
};

template<typename T>
class BackendAdapter
{
public:
    BackendAdapter(const std::string& back_name,
        std::function<PinDesc(const typename dag::Node<T>::Port&)> back2front);

    void InitNodePins(Node& node, const std::vector<PinDesc>& input,
        const std::vector<PinDesc>& output);
    void InitNodePins(Node& node, const std::string& name);

private:
    void InitPinsImpl(Node& node, bool is_input,
        const std::vector<PinDesc>& pins);

private:
    std::string m_back_name;

    std::function<PinDesc(const typename dag::Node<T>::Port&)> m_pin_back2front;

}; // BackendAdapter

}

#include "blueprint/BackendAdapter.inl"