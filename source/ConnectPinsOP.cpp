#include "blueprint/ConnectPinsOP.h"
#include "blueprint/CompNode.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/RenderSystem.h"
#include "blueprint/Connecting.h"
#include "blueprint/WxCreateNodeDlg.h"
#include "blueprint/MessageID.h"

#include <ee0/WxStagePage.h>
#include <ee0/CameraHelper.h>
#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>
#include <ee0/MsgHelper.h>

#include <SM_Calc.h>
#include <node0/SceneNode.h>
#include <node0/CompIdentity.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <painting2/PrimitiveDraw.h>

namespace bp
{

ConnectPinsOP::ConnectPinsOP(const std::shared_ptr<pt0::Camera>& cam, ee0::WxStagePage& stage,
	                         const std::vector<std::shared_ptr<node::Node>>& nodes)
	: ee0::EditOP(cam)
	, m_stage(stage)
	, m_nodes(nodes)
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
	if (ee0::EditOP::OnMouseLeftUp(x, y)) {
		return true;
	}

	if (m_selected)
	{
		if (QueryOrCreateNode(x, y)) {
			m_stage.GetSubjectMgr()->NotifyObservers(MSG_BLUE_PRINT_CHANGED);
		}
		m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
		return true;
	}

	return false;
}

bool ConnectPinsOP::OnMouseDrag(int x, int y)
{
	if (m_selected)
	{
		m_last_pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);

		sm::vec2 v0, v3;
		if (m_selected->IsInput()) {
			v0 = m_last_pos;
			v3 = m_first_pos;
		} else {
			v0 = m_first_pos;
			v3 = m_last_pos;
		}

		float d = fabs((v3.x - v0.x) * NodeLayout::CONNECTING_BEZIER_DIST);
		auto v1 = v0 + sm::vec2(d, 0);
		auto v2 = v3 - sm::vec2(d, 0);
		m_curve.SetCtrlPos(v0, v1, v2, v3);

		m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
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

	auto& input = cnode.GetNode()->GetAllInput();
	for (auto& p : input)
	{
		auto center = NodeLayout::GetPinsPos(*p);
		if (sm::dis_pos_to_pos(pos, center) < NodeLayout::PINS_RADIUS * 1.5f)
		{
			p_center = center;
			return p;
		}
	}

	auto& output = cnode.GetNode()->GetAllOutput();
	for (auto& p : output)
	{
		auto center = NodeLayout::GetPinsPos(*p);
		if (sm::dis_pos_to_pos(pos, center) < NodeLayout::PINS_RADIUS * 1.5f) {
			p_center = center;
			return p;
		}
	}

	return nullptr;
}

bool ConnectPinsOP::QueryOrCreateNode(int x, int y)
{
	bool dirty = false;

	std::shared_ptr<node::Pins> target = nullptr;
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	m_stage.Traverse([&](const ee0::GameObj& obj)->bool
	{
		auto& cbb = obj->GetUniqueComp<n2::CompBoundingBox>();
		if (!cbb.GetBounding(*obj).IsContain(pos)) {
			return true;
		}

		sm::vec2 center;
		target = QueryPinsByPos(obj, pos, center);
		return !target;
	});
	if (target)
	{
		if (m_selected->GetType() == target->GetType() &&
			m_selected->IsInput() != target->IsInput())
		{
			if (m_selected->IsInput()) {
				node::make_connecting(target, m_selected);
			} else {
				node::make_connecting(m_selected, target);
			}
			dirty = true;
		}
	}
	else
	{
		dirty = CreateNode(x, y);
	}

	m_selected = nullptr;
	m_curve.Clear();

	return dirty;
}

bool ConnectPinsOP::CreateNode(int x, int y)
{
	auto base = m_stage.GetScreenPosition();
	assert(m_selected);
	WxCreateNodeDlg dlg(&m_stage, base + wxPoint(x, y), *m_selected, m_nodes);
	if (dlg.ShowModal() != wxID_OK) {
		m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
		return false;
	}

	auto type = dlg.GetSelectedType();
	std::shared_ptr<node::Node> bp_node = nullptr;
	for (auto& n : m_nodes) {
		if (n->TypeName() == type) {
			bp_node = n->Create();
			break;
		}
	}
	assert(bp_node);
	auto& style = bp_node->GetStyle();

	auto node = std::make_shared<n0::SceneNode>();
	auto& cnode = node->AddSharedComp<bp::CompNode>(bp_node);
	cnode.GetNode()->SetParent(node);
	// trans
	auto& ctrans = node->AddUniqueComp<n2::CompTransform>();
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	pos.x += bp_node->GetStyle().width * 0.5f;
	pos.y -= bp_node->GetStyle().height * 0.5f;
	ctrans.SetPosition(*node, pos);
	// id
	node->AddUniqueComp<n0::CompIdentity>();
	// aabb
	node->AddUniqueComp<n2::CompBoundingBox>(
		sm::rect(style.width, style.height)
	);

	// connect
	auto pins_type = m_selected->GetType();
	if (m_selected->IsInput())
	{
		auto& output = bp_node->GetAllOutput();
		for (auto& pins : output) {
			if (pins->GetType() == pins_type) {
				node::make_connecting(pins, m_selected);
				break;
			}
		}
	} else {
		auto& input = bp_node->GetAllInput();
		for (auto& pins : input) {
			if (pins->GetType() == pins_type) {
				node::make_connecting(m_selected, pins);
				break;
			}
		}
	}

	ee0::MsgHelper::InsertNode(*m_stage.GetSubjectMgr(), node);

	std::vector<n0::NodeWithPos> nwps;
	nwps.push_back(n0::NodeWithPos(node, node, 0));
	ee0::MsgHelper::InsertSelection(*m_stage.GetSubjectMgr(), nwps);

	return true;
}

}