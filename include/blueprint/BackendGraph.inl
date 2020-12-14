#pragma once

#include "blueprint/Connecting.h"
#include "blueprint/Pin.h"

namespace bp
{

template <typename T>
BackendGraph<T>::BackendGraph(const std::string& back_name, const std::string& front_name,
                              const std::function<void(const bp::Node&, dag::Node<T>&)>& front2back)
    : m_back_name(back_name)
    , m_front_name(front_name)
    , m_front2back_cb(front2back)
{
    m_eval = std::make_shared<dag::Graph<T>>();
}

template <typename T>
void BackendGraph<T>::OnAddNode(const Node& front, bool need_update)
{
    auto back = CreateBackFromFront(front);
    if (!back) {
        return;
    }

    m_eval->AddNode(back);

    m_front2back.insert({ &front, back });

    if (front.get_type().is_derived_from<Node>()) {
        const_cast<Node&>(static_cast<const Node&>(front)).SetName(back->GetName());
    }

    UpdatePropBackFromFront(front, *back);
    if (need_update) {
        Update();
    }
}

template <typename T>
void BackendGraph<T>::OnRemoveNode(const Node& node)
{
    auto itr = m_front2back.find(&node);
    if (itr == m_front2back.end()) {
        return;
    }

    m_eval->RemoveNode(itr->second);
    m_front2back.erase(itr);

    Update();
}

template <typename T>
void BackendGraph<T>::OnClearAllNodes()
{
    m_eval->ClearAllNodes();
    m_front2back.clear();

    Update();
}

template <typename T>
void BackendGraph<T>::OnNodePropChanged(const NodePtr& node)
{
    auto itr = m_front2back.find(node.get());
    if (itr == m_front2back.end()) {
        return;
    }

    UpdatePropBackFromFront(*node, *itr->second);

    if (node->GetName() != itr->second->GetName()) {
        //m_eval->Rename(itr->second->GetName(), node->GetName());
        if (itr->second->GetName() != node->GetName()) {
            node->SetName(itr->second->GetName());
        }
    }

    m_eval->PropChanged(itr->second);
    //m_eval->MakeDirty();

    Update();
}

template <typename T>
void BackendGraph<T>::OnConnected(const Connecting& conn)
{
    auto f_pin = conn.GetFrom();
    auto t_pin = conn.GetTo();

    auto f_itr = m_front2back.find(&f_pin->GetParent());
    auto t_itr = m_front2back.find(&t_pin->GetParent());
    if (f_itr == m_front2back.end() || t_itr == m_front2back.end()) {
        return;
    }

    //if (t_itr->first->GetAllInput().size() > t_itr->second->GetImports().size()) {
    //    t_itr->second->AddInputPorts(t_itr->first->GetAllInput().size() - t_itr->first->GetAllOutput().size());
    //}

    m_eval->Connect(
        { f_itr->second, f_pin->GetPosIdx() },
        { t_itr->second, t_pin->GetPosIdx() }
    );

    Update();
}

template <typename T>
void BackendGraph<T>::OnDisconnecting(const Connecting& conn)
{
    auto f_pin = conn.GetFrom();
    auto t_pin = conn.GetTo();

    auto f_itr = m_front2back.find(&f_pin->GetParent());
    auto t_itr = m_front2back.find(&t_pin->GetParent());
    if (f_itr == m_front2back.end() || t_itr == m_front2back.end()) {
        return;
    }

    m_eval->Disconnect(
        { f_itr->second, f_pin->GetPosIdx() },
        { t_itr->second, t_pin->GetPosIdx() }
    );

    Update();
}

template <typename T>
void BackendGraph<T>::OnRebuildConnection()
{
    std::vector<std::pair<dag::Node<T>::PortAddr, dag::Node<T>::PortAddr>> conns;
    for (auto& itr : m_front2back)
    {
        auto& front = itr.first;
        auto& back  = itr.second;
        for (auto& in : front->GetAllInput())
        {
            for (auto& conn : in->GetConnecting())
            {
                auto f_pin = conn->GetFrom();
                auto t_pin = conn->GetTo();

                auto f_itr = m_front2back.find(&f_pin->GetParent());
                auto t_itr = m_front2back.find(&t_pin->GetParent());
                if (f_itr == m_front2back.end() || t_itr == m_front2back.end()) {
                    continue;
                }

                //if (t_itr->first->GetAllInput().size() > t_itr->second->GetImports().size()) {
                //    t_itr->second->AddInputPorts(t_itr->first->GetAllInput().size() - t_itr->first->GetAllOutput().size());
                //}

                conns.push_back({
                    { f_itr->second, f_pin->GetPosIdx() },
                    { t_itr->second, t_pin->GetPosIdx() }
                });
            }
        }
    }

    m_eval->RebuildConnections(conns);

    Update();
}

template <typename T>
std::shared_ptr<dag::Node<T>>
BackendGraph<T>::QueryBackNode(const Node& front_node) const
{
    auto itr = m_front2back.find(&front_node);
    return itr == m_front2back.end() ? nullptr : itr->second;
}

template <typename T>
void BackendGraph<T>::Update()
{
    if (m_update_enable && m_ctx) {
        m_eval->Update(m_ctx);
    }
}

template <typename T>
void BackendGraph<T>::UpdatePropBackFromFront(const bp::Node& front, dag::Node<T>& back)
{
    if (m_front2back_cb) {
        m_front2back_cb(front, back);
    }

    auto f_type = front.get_type();
    auto b_type = back.get_type();
    if (f_type.is_derived_from<Node>() &&
        b_type.is_derived_from<dag::Node<T>>())
    {
        for (auto& dst_prop : b_type.get_properties())
        {
            auto src_prop = f_type.get_property(dst_prop.get_name());
            assert(src_prop.is_valid());
            if (dst_prop.set_value(back, src_prop.get_value(front))) {
                back.SetDirty(true);
            }
        }
    }
}

template <typename T>
std::shared_ptr<dag::Node<T>>
BackendGraph<T>::CreateBackFromFront(const bp::Node& node)
{
    auto type = node.get_type();
    auto src_type = type.get_name().to_string();
    std::string dst_type;
    std::string lib_str = m_back_name;
    auto find_lib = src_type.find(m_front_name + "::");
    if (find_lib != std::string::npos) {
        dst_type = lib_str + "::" + src_type.substr(find_lib + (m_front_name + "::").size());
    }

    if (dst_type.empty()) {
        return nullptr;
    }

    std::shared_ptr<dag::Node<T>> dst = nullptr;

    if (!dst_type.empty())
    {
	    rttr::type t = rttr::type::get_by_name(dst_type);
        // fixme: specify node type
	    if (!t.is_valid())
        {
            //assert(0);
            return nullptr;
	    }
        else
        {
            rttr::variant var = t.create();
            assert(var.is_valid());

            dst = var.get_value<std::shared_ptr<dag::Node<T>>>();
            assert(dst);
        }
    }

    if (node.get_type().is_derived_from<Node>()) {
        dst->SetName(static_cast<const Node&>(node).GetName());
    }

    return dst;
}

}