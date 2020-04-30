#pragma once

#include <ee0/WxStagePage.h>

#include "blueprint/BackendGraph.h"

namespace n0 { class CompComplex; }

namespace bp
{

template <typename T>
class WxGraphPage : public ee0::WxStagePage
{
public:
	WxGraphPage(wxWindow* parent, const ee0::GameObj& root,
        const ee0::SubjectMgrPtr& preview_sub_mgr, uint32_t preview_update_msg,
        const std::string& back_name, const std::string& front_name);
    virtual ~WxGraphPage();

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
        const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

    auto& GetEval() const { return m_eval; }

protected:
    virtual void OnEvalChangeed() {}

private:
    bool ClearAllSceneObjs();
    bool InsertSceneObj(const ee0::VariantSet& variants);
    bool DeleteSceneObj(const ee0::VariantSet& variants);

    bool AfterInsertNodeConn(const ee0::VariantSet& variants);
    bool BeforeDeleteNodeConn(const ee0::VariantSet& variants);
    bool UpdateNodeProp(const ee0::VariantSet& variants);

    void UpdateAABB(const ee0::GameObj& obj);

    void InsertScenNode(n0::CompComplex& root,
        const n0::SceneNodePtr& node);

protected:
    std::shared_ptr<bp::BackendGraph<T>> m_eval = nullptr;

private:
    ee0::SubjectMgrPtr m_preview_sub_mgr    = nullptr;
    uint32_t           m_preview_update_msg = 0;

    n0::SceneNodePtr m_root = nullptr;

}; // WxGraphPage

}

#include "blueprint/WxGraphPage.inl"