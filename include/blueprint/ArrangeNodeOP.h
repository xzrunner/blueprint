#pragma once

#include <ee2/ArrangeNodeOP.h>

namespace bp
{

namespace node { class Commentary; }

class ArrangeNodeOP : public ee2::ArrangeNodeOP
{
public:
    ArrangeNodeOP(const std::shared_ptr<pt0::Camera>& camera,
        ee0::WxStagePage& stage, ECS_WORLD_PARAM const ee2::ArrangeNodeCfg& cfg,
        const std::shared_ptr<ee0::EditOP>& prev_op);

    virtual bool OnMouseLeftDown(int x, int y) override;
    virtual bool OnMouseLeftUp(int x, int y) override;
    virtual bool OnMouseMove(int x, int y) override;
    virtual bool OnMouseDrag(int x, int y) override;

    virtual bool OnDraw() const override;

private:
    enum class SelectType
    {
        Left,
        Right,
        Low,
        High,
        LeftLow,
        RightLow,
        RightHigh,
        LeftHigh,
    };

    struct Selected
    {
        void Reset() { 
            node.reset();
            comm.reset(); 
        }
        bool IsValid() const { return node && comm; }

        n0::SceneNodePtr node = nullptr;
        std::shared_ptr<node::Commentary> comm = nullptr;
        SelectType type;
    };

private:
    bool QueryCommNodeCornerByPos(const sm::vec2& pos, Selected& selected) const;

private:
    ee0::WxStagePage& m_stage;

    Selected m_hot;
    Selected m_active;

}; // ArrangeNodeOP

}