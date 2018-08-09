#include "blueprint/ConnectPinsOP.h"
#include "blueprint/CompNode.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/RenderSystem.h"

#include <ee0/WxStagePage.h>
#include <ee0/CameraHelper.h>
#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>

#include <SM_Calc.h>
#include <node0/SceneNode.h>
#include <node2/CompTransform.h>
#include <painting2/PrimitiveDraw.h>

namespace bp
{

ConnectPinsOP::ConnectPinsOP(const std::shared_ptr<pt0::Camera>& cam, ee0::WxStagePage& stage)
	: ee0::EditOP(cam)
	, m_stage(stage)
{
	m_first_pos.MakeInvalid();
	m_last_pos.MakeInvalid();
}

bool ConnectPinsOP::OnMouseLeftDown(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftDown(x, y)) {
		return true;
	}

	if (m_stage.GetSelection().Size() == 1)
	{
		n0::SceneNodePtr node = nullptr;
		m_stage.GetSelection().Traverse([&](const ee0::GameObjWithPos& nwp)->bool {
			node = nwp.GetNode();
			return false;
		});

		auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);

		m_first_pos.MakeInvalid();
		m_selected = QueryPinsByPos(node, pos, m_first_pos);
		m_last_pos = m_first_pos;
	}

	return false;
}

bool ConnectPinsOP::OnMouseLeftUp(int x, int y)
{
	if (m_selected) {

	} else {
		if (ee0::EditOP::OnMouseLeftUp(x, y)) {
			return true;
		}
	}

	return false;
}

bool ConnectPinsOP::OnMouseDrag(int x, int y)
{
	if (m_selected)
	{
		m_last_pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
		m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
		float d = (m_last_pos.x - m_first_pos.x) * 0.3f;
		m_curve.SetCtrlPos(m_first_pos, m_first_pos + sm::vec2(d, 0), m_last_pos - sm::vec2(d, 0), m_last_pos);
	}
	else
	{
		if (ee0::EditOP::OnMouseDrag(x, y)) {
			return true;
		}
	}

	return false;
}

bool ConnectPinsOP::OnDraw() const
{
	if (m_selected)
	{
		if (m_first_pos.IsValid() && m_last_pos.IsValid())
		{
			pt2::PrimitiveDraw::SetColor(RenderSystem::GetPinsColor(m_selected->GetType()));
			pt2::PrimitiveDraw::Polyline(nullptr, m_curve.GetVertices(), false);
		}
	}
	else
	{
		if (ee0::EditOP::OnDraw()) {
			return true;
		}
	}

	return false;
}

std::shared_ptr<node::Pins> ConnectPinsOP::QueryPinsByPos(const n0::SceneNodePtr& node,
	                                                      const sm::vec2& pos, sm::vec2& p_center)
{
	if (!node->HasSharedComp<CompNode>()) {
		return nullptr;
	}

	auto& ctrans = node->GetUniqueComp<n2::CompTransform>();
	auto& mat = ctrans.GetTrans().GetMatrix();

	auto& cnode = node->GetSharedComp<CompNode>();
	auto& bp_node = cnode.GetNode();

	auto& style = bp_node->GetStyle();
	float hw = style.width  * 0.5f;
	float hh = style.height * 0.5f;

	float y = hh - NodeLayout::DEFAULT_HEIGHT;

	auto& input = cnode.GetNode()->GetAllInput();
	float curr_y = y;
	for (auto& p : input)
	{
		auto center = mat * sm::vec2(-hw + NodeLayout::PINS_RADIUS * 2, curr_y - NodeLayout::DEFAULT_HEIGHT * 0.5f);
		if (sm::dis_pos_to_pos(pos, center) < NodeLayout::PINS_RADIUS) {
			p_center = center;
			return p;
		}
		curr_y -= NodeLayout::DEFAULT_HEIGHT;
	}

	auto& output = cnode.GetNode()->GetAllOutput();
	curr_y = y;
	for (auto& p : output)
	{
		auto center = mat * sm::vec2(hw - NodeLayout::PINS_RADIUS * 2, curr_y - NodeLayout::DEFAULT_HEIGHT * 0.5f);
		if (sm::dis_pos_to_pos(pos, center) < NodeLayout::PINS_RADIUS) {
			p_center = center;
			return p;
		}
		curr_y -= NodeLayout::DEFAULT_HEIGHT;
	}

	return nullptr;
}

}