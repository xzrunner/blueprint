#pragma once

#include <ee2/TranslateNodeState.h>

#include <SM_Rect.h>

namespace ee0 { class WxStagePage; }

namespace bp
{

class Node;

class TranslateNodeState : public ee2::TranslateNodeState
{
public:
    TranslateNodeState(ee0::WxStagePage& stage,
        const std::shared_ptr<pt0::Camera>& camera);

    virtual bool OnMouseRelease(int x, int y) override;

    virtual bool Clear() override;

protected:
    virtual void Translate(const sm::vec2& offset) override;

private:
    void Translate(const std::vector<std::weak_ptr<Node>>& nodes,
        const sm::vec2& offset) const;

    std::vector<n0::SceneNodePtr> QueryCommNodeByRect(const sm::rect& r) const;

    bool UpdateSelectionCommentary() const;
    bool UpdateNodeCommentary(const n0::SceneNodePtr& node,
        const sm::rect& src, const sm::rect& dst) const;

    void GetSelectedAABBs(std::vector<sm::rect>& aabbs) const;

private:
    ee0::WxStagePage& m_stage;

    std::vector<sm::rect> m_selected_aabbs;

}; // TranslateNodeState

}