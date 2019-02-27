#pragma once

#include <ee2/TranslateNodeState.h>

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

protected:
    virtual void Translate(const sm::vec2& offset) override;

private:
    void Translate(const std::vector<std::weak_ptr<Node>>& nodes,
        const sm::vec2& offset) const;

    std::vector<n0::SceneNodePtr> QueryCommNodeByPos(const sm::vec2& pos) const;

    void AddSelectionToCommentary(const sm::vec2& from, const sm::vec2& to) const;

private:
    ee0::WxStagePage& m_stage;

}; // TranslateNodeState

}