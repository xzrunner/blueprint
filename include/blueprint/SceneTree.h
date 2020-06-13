#pragma once

#include "blueprint/BackendGraph.h"

#include <node0/typedef.h>

#include <vector>

namespace bp
{

template <typename T>
class SceneTree
{
public:
    SceneTree(const std::string& back_name, const std::string& front_name,
        const std::function<void(const bp::Node&, dag::Node<T>&)>& front2back);

    bool Add(const n0::SceneNodePtr& node);
    bool Remove(const n0::SceneNodePtr& node);
    bool Clear();

    bool Push(const n0::SceneNodePtr& node);
    n0::SceneNodePtr Pop();
    bool SetDepth(size_t depth);

    auto GetCurrNode() const { return m_path.empty() ? nullptr : m_path.back(); }
    std::shared_ptr<BackendGraph<T>> GetCurrEval() const;
    static std::shared_ptr<BackendGraph<T>> GetEval(const n0::SceneNodePtr& node);

    n0::SceneNodePtr GetRootNode() const { return m_path.empty() ? nullptr : m_path.front(); }

    void SetRootGraph(const std::shared_ptr<BackendGraph<T>>& root_graph) {
        m_root_graph = root_graph;
    }

    auto GetFrontToBackCB() const { return m_front2back_cb; }

private:
    bool IsCurrChild(const n0::SceneNodePtr& node) const;

    void SetupCurrNode();

private:
    std::string m_back_name, m_front_name;

    std::vector<n0::SceneNodePtr> m_path;

    std::function<void(const bp::Node&, dag::Node<T>&)> m_front2back_cb = nullptr;

    std::shared_ptr<BackendGraph<T>> m_root_graph = nullptr;

}; // SceneTree

}

#include "blueprint/SceneTree.inl"