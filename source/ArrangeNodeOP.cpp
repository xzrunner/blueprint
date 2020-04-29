#include "blueprint/ArrangeNodeOP.h"
#include "blueprint/TranslateNodeState.h"
#include "blueprint/CompNode.h"
#include "blueprint/Node.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/node/Commentary.h"

#include <ee0/WxStagePage.h>
#include <ee0/CameraHelper.h>

#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompTransform.h>
#include <unirender/Factory.h>
#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>
#include <painting2/OrthoCamera.h>
#include <SM_Calc.h>

namespace bp
{

ArrangeNodeOP::ArrangeNodeOP(const std::shared_ptr<pt0::Camera>& camera,
                             ee0::WxStagePage& stage, ECS_WORLD_PARAM const ee2::ArrangeNodeCfg& cfg,
                             const std::shared_ptr<ee0::EditOP>& prev_op)
    : ee2::ArrangeNodeOP(camera, stage, cfg, prev_op)
    , m_stage(stage)
{
    m_impl->SetTranslateOP(std::make_shared<TranslateNodeState>(stage, camera));
}

bool ArrangeNodeOP::OnMouseLeftDown(int x, int y)
{
    if (!m_hot.IsValid()) {
        return ee2::ArrangeNodeOP::OnMouseLeftDown(x, y);
    } else {
        m_active = m_hot;
        return false;
    }
}

bool ArrangeNodeOP::OnMouseLeftUp(int x, int y)
{
    if (!m_hot.IsValid()) {
        return ee2::ArrangeNodeOP::OnMouseLeftUp(x, y);
    } else {
        m_active.Reset();
        return false;
    }
}

bool ArrangeNodeOP::OnMouseMove(int x, int y)
{
    if (ee2::ArrangeNodeOP::OnMouseMove(x, y)) {
        return true;
    }

    auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);

    m_hot.Reset();
    QueryCommNodeCornerByPos(pos, m_hot);

    return false;
}

bool ArrangeNodeOP::OnMouseDrag(int x, int y)
{
    if (!m_active.IsValid()) {
        return ee2::ArrangeNodeOP::OnMouseDrag(x, y);
    }

    auto st = m_active.comm->GetStyle();

    auto& c = m_active.comm->GetPos();
    const auto hw = st.width * 0.5f;
    const auto hh = st.height * 0.5f;

    auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);

    float new_w = st.width;
    float new_h = st.height;
    sm::vec2 new_c = c;
    if (m_active.type == SelectType::LeftLow ||
        m_active.type == SelectType::RightLow ||
        m_active.type == SelectType::RightHigh ||
        m_active.type == SelectType::LeftHigh)
    {
        sm::vec2 other_pos;
        switch (m_active.type)
        {
        case SelectType::LeftLow:
            other_pos = c + sm::vec2(hw, hh);
            break;
        case SelectType::RightLow:
            other_pos = c + sm::vec2(-hw, hh);
            break;
        case SelectType::RightHigh:
            other_pos = c + sm::vec2(-hw, -hh);
            break;
        case SelectType::LeftHigh:
            other_pos = c + sm::vec2(hw, -hh);
            break;
        }
        new_w = std::fabs(pos.x - other_pos.x);
        new_h = std::fabs(pos.y - other_pos.y);
        new_c = (pos + other_pos) * 0.5f;
    }
    else
    {
        switch (m_active.type)
        {
        case SelectType::Left:
            new_w = c.x + hw - pos.x;
            new_c.x = c.x + hw - new_w * 0.5f;
            break;
        case SelectType::Right:
            new_w = pos.x - c.x + hw;
            new_c.x = c.x - hw + new_w * 0.5f;
            break;
        case SelectType::High:
            new_h = pos.y - c.y + hh;
            new_c.y = c.y - hh + new_h * 0.5f;
            break;
        case SelectType::Low:
            new_h = c.y + hh - pos.y;
            new_c.y = c.y + hh - new_h * 0.5f;
            break;
        }
    }

    m_active.comm->SetWidth(new_w);
    m_active.comm->SetHeight(new_h);

    m_active.comm->SetPos(new_c);

    m_active.comm->SetSizeChanged(true);

    auto& ctrans = m_active.node->GetUniqueComp<n2::CompTransform>();
    ctrans.SetPosition(*m_active.node, new_c);

    return false;
}

bool ArrangeNodeOP::QueryCommNodeCornerByPos(const sm::vec2& pos, Selected& selected) const
{
    bool ret = false;

    float radius = 5.0f;
    if (m_camera->TypeID() == pt0::GetCamTypeID<pt2::OrthoCamera>()) {
        radius *= std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera)->GetScale();
    }

    m_stage.Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (!obj->HasUniqueComp<CompNode>()) {
            return true;
        }
        auto node = obj->GetUniqueComp<CompNode>().GetNode();
        if (node->get_type() != rttr::type::get<node::Commentary>()) {
            return true;
        }

        auto& c = node->GetPos();
        const auto hw = node->GetStyle().width * 0.5f;
        const auto hh = node->GetStyle().height * 0.5f;

        if (sm::dis_pos_to_pos(c, pos) > std::sqrt(hw * hw + hh * hh) + radius) {
            return true;
        }

        bool find = false;
        if (sm::dis_pos_to_pos(pos, c + sm::vec2(-hw, -hh)) < radius) {
            selected.type = SelectType::LeftLow;
            find = true;
        } else if (sm::dis_pos_to_pos(pos, c + sm::vec2(hw, -hh)) < radius) {
            selected.type = SelectType::RightLow;
            find = true;
        } else if (sm::dis_pos_to_pos(pos, c + sm::vec2(hw, hh)) < radius) {
            selected.type = SelectType::RightHigh;
            find = true;
        } else if (sm::dis_pos_to_pos(pos, c + sm::vec2(-hw, hh)) < radius) {
            selected.type = SelectType::LeftHigh;
            find = true;
        } else if (std::fabs(pos.x - (c.x - hw)) < radius) {
            selected.type = SelectType::Left;
            find = true;
        } else if (std::fabs(pos.x - (c.x + hw)) < radius) {
            selected.type = SelectType::Right;
            find = true;
        } else if (std::fabs(pos.y - (c.y - hh)) < radius) {
            selected.type = SelectType::Low;
            find = true;
        } else if (std::fabs(pos.y - (c.y + hh)) < radius) {
            selected.type = SelectType::High;
            find = true;
        }
        if (find) {
            selected.node = obj;
            selected.comm = std::static_pointer_cast<node::Commentary>(node);
            ret = true;
            return false;
        } else {
            return true;
        }
    });

    return ret;
}

bool ArrangeNodeOP::OnDraw(const ur::Device& dev, ur::Context& ctx) const
{
    if (ee2::ArrangeNodeOP::OnDraw(dev, ctx)) {
        return true;
    }

    if (m_hot.IsValid())
    {
        auto& c = m_hot.comm->GetPos();
        auto& st = m_hot.comm->GetStyle();
        const float hw = st.width * 0.5f;
        const float hh = st.height * 0.5f;

        tess::Painter pt;
        float line_width = tess::DEFAULT_LINE_WIDTH;
        if (m_camera->TypeID() == pt0::GetCamTypeID<pt2::OrthoCamera>()) {
            line_width *= std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera)->GetScale();
        }
        pt.AddRect(sm::vec2(-hw, -hh) + c, sm::vec2(hw, hh) + c, 0xff000000, line_width);

        pt2::RenderSystem::DrawPainter(dev, ctx, ur::DefaultRenderState2D(), pt);
    }

    return false;
}

}