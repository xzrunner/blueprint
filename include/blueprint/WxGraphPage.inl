#pragma once

#include "blueprint/MessageID.h"
#include "blueprint/Blueprint.h"
#include "blueprint/CompNode.h"
#include "blueprint/Node.h"
#include "blueprint/MessageID.h"
#include "blueprint/node/SubGraph.h"

#include <ee0/MessageID.h>
#include <ee0/SubjectMgr.h>

#include <guard/check.h>
#include <node0/CompComplex.h>
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>

namespace
{

const uint32_t MESSAGES[] =
{
    ee0::MSG_SCENE_NODE_INSERT,
    ee0::MSG_SCENE_NODE_DELETE,
    ee0::MSG_SCENE_NODE_CLEAR,

    bp::MSG_BP_CONN_INSERT,
    bp::MSG_BP_CONN_DELETE,
    bp::MSG_BP_CONN_REBUILD,
    bp::MSG_BP_NODE_PROP_CHANGED,
    bp::MSG_BP_DISABLE_UPDATE_EVAL,
    bp::MSG_BP_ENABLE_UPDATE_EVAL,
    bp::MSG_BP_SCENE_ROOT_TO_NEXT_LEVEL,
    bp::MSG_BP_SCENE_ROOT_SEEK_TO_PREV_LEVEL,
};

}

namespace bp
{

template <typename T>
WxGraphPage<T>::WxGraphPage(wxWindow* parent, const ee0::GameObj& root,
                            const ee0::SubjectMgrPtr& preview_sub_mgr, uint32_t preview_update_msg,
                            const std::string& back_name, const std::string& front_name)
    : ee0::WxStagePage(parent)
    , m_preview_sub_mgr(preview_sub_mgr)
    , m_preview_update_msg(preview_update_msg)
    , m_root(root)
{
    Blueprint::Instance();

    m_stree = std::make_shared<SceneTree<T>>(back_name, front_name);
    m_stree->Add(root);

    for (auto& msg : MESSAGES) {
        m_sub_mgr->RegisterObserver(msg, this);
    }
}

template <typename T>
WxGraphPage<T>::~WxGraphPage()
{
    for (auto& msg : MESSAGES) {
        m_sub_mgr->UnregisterObserver(msg, this);
    }
}

template <typename T>
void WxGraphPage<T>::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
    ee0::WxStagePage::OnNotify(msg, variants);

	bool dirty = false;
	switch (msg)
	{
	case ee0::MSG_SCENE_NODE_INSERT:
		dirty = InsertSceneObj(variants);
		break;
	case ee0::MSG_SCENE_NODE_DELETE:
		dirty = DeleteSceneObj(variants);
		break;
	case ee0::MSG_SCENE_NODE_CLEAR:
        dirty = ClearAllSceneObjs();
		break;

    case MSG_BP_CONN_INSERT:
        dirty = AfterInsertNodeConn(variants);
        break;
    case MSG_BP_CONN_DELETE:
        dirty = BeforeDeleteNodeConn(variants);
        break;
    case MSG_BP_CONN_REBUILD:
        m_stree->GetCurrEval()->OnRebuildConnection();
        dirty = true;
        break;
    case MSG_BP_NODE_PROP_CHANGED:
        dirty = UpdateNodeProp(variants);
        break;
    case MSG_BP_DISABLE_UPDATE_EVAL:
        m_stree->GetCurrEval()->EnableUpdate(false);
        break;
    case MSG_BP_ENABLE_UPDATE_EVAL:
        m_stree->GetCurrEval()->EnableUpdate(true);
        break;
    case MSG_BP_SCENE_ROOT_TO_NEXT_LEVEL:
        dirty = PathPushToNext(variants);
        break;
    case MSG_BP_SCENE_ROOT_SEEK_TO_PREV_LEVEL:
        dirty = PathPopToPrev(variants);
        break;
	}

	if (dirty)
    {
        m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
        m_preview_sub_mgr->NotifyObservers(m_preview_update_msg);

        OnEvalChangeed();
	}
}

template <typename T>
void WxGraphPage<T>::Traverse(std::function<bool(const ee0::GameObj&)> func,
                           const ee0::VariantSet& variants , bool inverse) const
{
    m_root->GetSharedComp<n0::CompComplex>().Traverse(func, inverse);
}

template <typename T>
bool WxGraphPage<T>::ClearAllSceneObjs()
{
    // update scene node
    assert(m_stree->GetCurrNode()->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = m_stree->GetCurrNode()->GetSharedComp<n0::CompComplex>();
    bool dirty = !ccomplex.GetAllChildren().empty();
    ccomplex.RemoveAllChildren();

    // front eval cb
    m_stree->GetCurrEval()->OnClearAllNodes();

    return true;
}

template <typename T>
bool WxGraphPage<T>::InsertSceneObj(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");

    // update scene node
    assert(m_stree->GetCurrNode()->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = m_stree->GetCurrNode()->GetSharedComp<n0::CompComplex>();
    InsertScenNode(ccomplex, *obj);

    if ((*obj)->HasUniqueComp<bp::CompNode>())
    {
        auto& bp_node = (*obj)->GetUniqueComp<bp::CompNode>().GetNode();

        // prepare ccomplex
        auto type = bp_node->get_type();
        if (type.is_derived_from<node::SubGraph>())
        {
            if (!(*obj)->HasSharedComp<n0::CompComplex>()) {
                (*obj)->AddSharedComp<n0::CompComplex>();
            }
        }
    }

    return true;
}

template <typename T>
bool WxGraphPage<T>::DeleteSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

    // update scene (*obj)
    assert(m_stree->GetCurrNode()->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = m_stree->GetCurrNode()->GetSharedComp<n0::CompComplex>();
    bool dirty = ccomplex.RemoveChild(*obj);

    if (dirty && (*obj)->HasUniqueComp<CompNode>()) {
        auto& bp_node = (*obj)->GetUniqueComp<CompNode>().GetNode();
        m_stree->GetCurrEval()->OnRemoveNode(*bp_node);
    }

    return dirty;
}

template <typename T>
bool WxGraphPage<T>::AfterInsertNodeConn(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("conn");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: conn");
    const std::shared_ptr<Connecting>* conn = static_cast<const std::shared_ptr<Connecting>*>(var.m_val.pv);
    GD_ASSERT(conn, "err conn");

    m_stree->GetCurrEval()->OnConnected(**conn);

    return true;
}

template <typename T>
bool WxGraphPage<T>::BeforeDeleteNodeConn(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("conn");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: conn");
    const std::shared_ptr<Connecting>* conn = static_cast<const std::shared_ptr<Connecting>*>(var.m_val.pv);
    GD_ASSERT(conn, "err conn");

    assert(conn);
    m_stree->GetCurrEval()->OnDisconnecting(**conn);

    return true;
}

template <typename T>
bool WxGraphPage<T>::UpdateNodeProp(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    if (var.m_type == ee0::VT_EMPTY) {
        return false;
    }

    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");

    if ((*obj)->HasUniqueComp<CompNode>())
    {
        auto& bp_node = (*obj)->GetUniqueComp<CompNode>().GetNode();
        m_stree->GetCurrEval()->OnNodePropChanged(bp_node);
        UpdateAABB(*obj);

        return true;
    }
    else
    {
        return false;
    }
}

template <typename T>
bool WxGraphPage<T>::PathPushToNext(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");


    if (m_root == *obj) {
        return false;
    }

    m_stree->Push(*obj);

    m_root = *obj;
    m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
    return true;
}

template <typename T>
bool WxGraphPage<T>::PathPopToPrev(const ee0::VariantSet& variants)
{
    auto var_depth = variants.GetVariant("depth");
    GD_ASSERT(var_depth.m_type == ee0::VT_ULONG, "no var in vars: obj");
    auto depth = var_depth.m_val.ul;
    if (!m_stree->SetDepth(depth)) {
        return false;
    }

    m_root = m_stree->GetCurrNode();

    m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);

    return true;
}

template <typename T>
void WxGraphPage<T>::UpdateAABB(const ee0::GameObj& obj)
{
    auto& bp_node = obj->GetUniqueComp<CompNode>().GetNode();

    auto& st = bp_node->GetStyle();
    obj->GetUniqueComp<n2::CompBoundingBox>().SetSize(
        *obj, sm::rect(st.width, st.height)
    );
}

template <typename T>
void WxGraphPage<T>::InsertScenNode(n0::CompComplex& root,
                                 const n0::SceneNodePtr& node)
{
    root.AddChild(node);

    if (!node->HasUniqueComp<CompNode>()) {
        return;
    }

    auto& bp_node = node->GetUniqueComp<CompNode>().GetNode();
    m_stree->GetCurrEval()->OnAddNode(*bp_node);
    UpdateAABB(node);
}

}