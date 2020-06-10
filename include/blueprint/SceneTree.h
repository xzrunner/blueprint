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
    SceneTree(const std::string& back_name,
        const std::string& front_name);

    bool Add(const n0::SceneNodePtr& node);
    bool Remove(const n0::SceneNodePtr& node);
    bool Clear();

    bool Push(const n0::SceneNodePtr& node);
    n0::SceneNodePtr Pop();
    bool SetDepth(size_t depth);

    auto GetCurrEval() const { return m_path.parts.empty() ? nullptr : m_path.parts.back().eval; }
    auto GetCurrNode() const { return m_path.parts.empty() ? nullptr : m_path.parts.back().node; }

    void SetFront2BackCB(const std::function<void(const bp::Node&, dag::Node<T>&)>& front2back);

    n0::SceneNodePtr GetRootNode() const { return m_path.parts.empty() ? nullptr : m_path.parts[0].node; }

private:
    bool IsCurrChild(const n0::SceneNodePtr& node) const;

    void SetupCurrNode();

private:
    typedef std::shared_ptr<BackendGraph<T>> EvaluatorPtr;

    template <typename T>
    struct PathPart
    {
        PathPart(const n0::SceneNodePtr& node, const EvaluatorPtr& eval)
            : node(node), eval(eval) {}

        n0::SceneNodePtr node = nullptr;
        EvaluatorPtr     eval = nullptr;
    };

    template <typename T>
    struct Path
    {
        std::vector<PathPart<T>> parts;
    };

private:
    std::string m_back_name, m_front_name;

    Path<T> m_path;

    std::map<n0::SceneNodePtr, EvaluatorPtr> m_eval_cache;

    std::function<void(const bp::Node&, dag::Node<T>&)> m_front2back_cb = nullptr;

}; // SceneTree

}

#include "blueprint/SceneTree.inl"