#pragma once

#include "blueprint/node/SubGraph.h"

#include <node0/NodeFlags.h>
#include <node0/NodeFlagsHelper.h>

namespace bp
{

template <typename T>
SceneTree<T>::SceneTree(const std::string& back_name, const std::string& front_name)
    : m_back_name(back_name)
    , m_front_name(front_name)
{
}

template <typename T>
bool SceneTree<T>::Add(const n0::SceneNodePtr& node)
{
    if (m_path.parts.empty())
    {
        auto eval = std::make_shared<BackendGraph<T>>(m_back_name, m_front_name);
        eval->SetFront2BackCB(m_front2back_cb);

        m_eval_cache.insert({ node, eval });
        m_path.parts.push_back(PathPart<T>(node, eval));

        return true;
    }

    assert(!m_path.parts.empty());

    //// reset flags
    //ClearNodeDisplayTag();

    // update scene node
    auto& curr = m_path.parts.back();
    assert(curr.node->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
    ccomplex.AddChild(node);

    if (node->HasUniqueComp<bp::CompNode>())
    {
        auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
        auto& curr = m_path.parts.back();

        // update front
        if (curr.node->HasUniqueComp<bp::CompNode>())
        {
            auto bp_parent = curr.node->GetUniqueComp<bp::CompNode>().GetNode();
            if (bp_parent && bp_parent->get_type().is_derived_from<node::SubGraph<T>>()) {
                std::static_pointer_cast<node::SubGraph<T>>(bp_parent)->AddChild(bp_node);
            }
        }

        // front eval cb
        m_path.parts.back().eval->OnAddNode(*bp_node);

        //// update back
        //if (m_path.parts.size() > 1)
        //{
        //    auto& prev_eval = m_path.parts[m_path.parts.size() - 2].eval;
        //    auto& curr_node = m_path.parts.back().node;
        //    assert(curr_node->HasUniqueComp<bp::CompNode>());
        //    auto parent = prev_eval->QueryBackNode(*curr_node->GetUniqueComp<bp::CompNode>().GetNode());
        //    auto child = m_path.parts.back().eval->QueryBackNode(*bp_node);
        //    assert(parent->get_type().is_derived_from<sop::node::SubGraph<T>>());
        //    sop::node::SubGraph<T>::AddChild(std::static_pointer_cast<sop::node::SubGraph<T>>(parent), child);
        //}

        // prepare ccomplex
        auto type = bp_node->get_type();
        if (type.is_derived_from<node::SubGraph<T>>())
        {
            if (!node->HasSharedComp<n0::CompComplex>()) {
                node->AddSharedComp<n0::CompComplex>();
            }
        }

        //// update flags
        //if (type.is_derived_from<Node>()) {
        //    auto sopv_node = std::static_pointer_cast<Node>(bp_node);
        //}
    }

    return true;
}

template <typename T>
bool SceneTree<T>::Remove(const n0::SceneNodePtr& node)
{
    if (m_path.parts.empty()) {
        return false;
    }

    if (node == m_path.parts.front().node)
    {
        return Clear();
    }
    else
    {
        // update scene node
        auto& curr = m_path.parts.back();
        assert(curr.node->HasSharedComp<n0::CompComplex>());
        auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
        bool dirty = ccomplex.RemoveChild(node);

        if (dirty && node->HasUniqueComp<bp::CompNode>())
        {
            // update front
            if (curr.node->HasUniqueComp<bp::CompNode>())
            {
                auto bp_parent = curr.node->GetUniqueComp<bp::CompNode>().GetNode();
                if (bp_parent && bp_parent->get_type().is_derived_from<node::SubGraph<T>>()) {
                    auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
                    std::static_pointer_cast<node::SubGraph<T>>(bp_parent)->RemoveChild(bp_node);
                }
            }

            // front eval cb
            auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
            curr.eval->OnRemoveNode(*bp_node);

            //// update back
            //if (curr.node->HasUniqueComp<bp::CompNode>() && m_path.parts.size() > 1)
            //{
            //    auto bp_parent = curr.node->GetUniqueComp<bp::CompNode>().GetNode();
            //    if (bp_parent && bp_parent->get_type().is_derived_from<node::SubGraph<T>>())
            //    {
            //        auto subnetwork = std::static_pointer_cast<node::SubGraph<T>>(bp_parent);

            //        auto& prev_eval = m_path.parts[m_path.parts.size() - 2].eval;
            //        auto& curr_node = m_path.parts.back().node;
            //        assert(curr_node->HasUniqueComp<bp::CompNode>());
            //        auto parent = prev_eval->QueryBackNode(*curr_node->GetUniqueComp<bp::CompNode>().GetNode());
            //        auto child = m_path.parts.back().eval->QueryBackNode(*bp_node);
            //        assert(parent->get_type().is_derived_from<node::SubGraph<T>>());
            //        RebuildBackFromFront(std::static_pointer_cast<node::SubGraph<T>>(parent), subnetwork, *curr.eval);
            //    }
            //}
        }

        return dirty;
    }
}

template <typename T>
bool SceneTree<T>::Clear()
{
    if (m_path.parts.empty()) {
        return false;
    }

    // update scene node
    auto& curr = m_path.parts.back();
    assert(curr.node->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
    bool dirty = !ccomplex.GetAllChildren().empty();
	ccomplex.RemoveAllChildren();

    // update front
    if (curr.node->HasUniqueComp<bp::CompNode>())
    {
        auto bp_parent = curr.node->GetUniqueComp<bp::CompNode>().GetNode();
        if (bp_parent && bp_parent->get_type().is_derived_from<node::SubGraph<T>>()) {
            std::static_pointer_cast<node::SubGraph<T>>(bp_parent)->ClearAllChildren();
        }
    }

    // front eval cb
    assert(curr.eval);
    curr.eval->OnClearAllNodes();

    // update back
    if (m_path.parts.size() > 1)
    {
        auto& prev_eval = m_path.parts[m_path.parts.size() - 2].eval;
        auto& curr_node = m_path.parts.back().node;
        assert(curr_node->HasUniqueComp<bp::CompNode>());
        auto parent = prev_eval->QueryBackNode(*curr_node->GetUniqueComp<bp::CompNode>().GetNode());
        assert(parent->get_type().is_derived_from<node::SubGraph<T>>());
        std::static_pointer_cast<node::SubGraph<T>>(parent)->ClearChildren();
    }

    return dirty;
}

template <typename T>
bool SceneTree<T>::Push(const n0::SceneNodePtr& node)
{
    if (!node->HasUniqueComp<bp::CompNode>()) {
        return false;
    }

    auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
    auto type = bp_node->get_type();
    if (!type.is_derived_from<node::SubGraph<T>>()) {
        return false;
    }

    assert(IsCurrChild(node));
    auto itr = m_eval_cache.find(node);
    if (itr == m_eval_cache.end())
    {
        auto eval = std::make_shared<BackendGraph<T>>(m_back_name, m_front_name);
        if (node->HasSharedComp<n0::CompComplex>())
        {
            for (auto& c : node->GetSharedComp<n0::CompComplex>().GetAllChildren()) {
                if (c->HasUniqueComp<bp::CompNode>()) {
                    auto& bp_node = c->GetUniqueComp<bp::CompNode>().GetNode();
                    eval->OnAddNode(*bp_node, false);
                }
            }

            //if (node->HasUniqueComp<bp::CompNode>())
            //{
            //    auto bp_parent = node->GetUniqueComp<bp::CompNode>().GetNode();
            //    if (bp_parent->get_type().is_derived_from<node::SubGraph<T>>())
            //    {
            //        auto src = std::static_pointer_cast<node::SubGraph<T>>(bp_parent);
            //        auto dst = GetCurrEval()->QueryBackNode(*src);
            //        assert(dst && dst->get_type().is_derived_from<node::SubGraph<T>>());
            //        auto dst_sub_nw = std::static_pointer_cast<node::SubGraph<T>>(dst);
            //        RebuildBackFromFront(dst_sub_nw, src, *eval);
            //    }
            //}
        }

        m_eval_cache.insert({ node, eval });
        m_path.parts.push_back({ node, eval });

        eval->OnRebuildConnection();
    }
    else
    {
        m_path.parts.push_back({ node, itr->second });
    }

    SetupCurrNode();

    return true;
}

template <typename T>
n0::SceneNodePtr SceneTree<T>::Pop()
{
    if (m_path.parts.empty()) {
        return nullptr;
    }

    auto ret = m_path.parts.back().node;
    m_path.parts.pop_back();

    SetupCurrNode();

    return ret;
}

template <typename T>
bool SceneTree<T>::SetDepth(size_t depth)
{
    if (depth >= m_path.parts.size()) {
        return false;
    }

    m_path.parts.erase(m_path.parts.begin() + depth + 1, m_path.parts.end());

    SetupCurrNode();

    return true;
}

template <typename T>
void SceneTree<T>::SetFront2BackCB(const std::function<void(const bp::Node&, dag::Node<T>&)>& front2back)
{
    m_front2back_cb = front2back;

    for (auto& p : m_path.parts) {
        p.eval->SetFront2BackCB(m_front2back_cb);
    }
}

template <typename T>
bool SceneTree<T>::IsCurrChild(const n0::SceneNodePtr& node) const
{
    if (m_path.parts.empty()) {
        return false;
    }

    auto curr = m_path.parts.back();
    assert(curr.node->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
    for (auto& child : ccomplex.GetAllChildren()) {
        if (child == node) {
            return true;
        }
    }
    return false;
}

template <typename T>
void SceneTree<T>::SetupCurrNode()
{
    if (m_path.parts.empty()) {
        return;
    }

    auto curr = m_path.parts.back();
    assert(curr.node->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
    for (auto& c : ccomplex.GetAllChildren())
    {
        n0::NodeFlagsHelper::SetFlag<n0::NodeNotVisible>(*c, false);
        if (c->HasSharedComp<n0::CompComplex>()) {
            for (auto& cc : c->GetSharedComp<n0::CompComplex>().GetAllChildren()) {
                n0::NodeFlagsHelper::SetFlag<n0::NodeNotVisible>(*cc, true);
            }
        }
    }
}

}