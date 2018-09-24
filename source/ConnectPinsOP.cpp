#include "blueprint/ConnectPinsOP.h"
#include "blueprint/CompNode.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/RenderSystem.h"
#include "blueprint/Connecting.h"
#include "blueprint/WxCreateNodeDlg.h"
#include "blueprint/MessageID.h"
#include "blueprint/NodeBuilder.h"
#include "blueprint/NodeFactory.h"

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

namespace
{

const pt2::Color COL_SELECTED = pt2::Color(255, 0, 0);

const float QUERY_REGION = 5;

}

namespace bp
{

ConnectPinsOP::ConnectPinsOP(const std::shared_ptr<pt0::Camera>& cam, ee0::WxStagePage& stage,
	                         const std::vector<NodePtr>& nodes)
	: ee0::EditOP(cam)
	, m_stage(stage)
	, m_nodes(nodes)
{
	m_first_pos.MakeInvalid();
	m_last_pos.MakeInvalid();
}

bool ConnectPinsOP::OnKeyDown(int key_code)
{
	if (ee0::EditOP::OnKeyDown(key_code)) {
		return true;
	}

	if (key_code == WXK_DELETE)
	{
		for (auto& conn : m_selected_conns) {
			disconnect(conn);
		}
		m_selected_conns.clear();
	}

	return false;
}

bool ConnectPinsOP::OnMouseLeftDown(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftDown(x, y)) {
		return true;
	}

	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	m_first_pos = pos;
	m_last_pos = m_first_pos;

	// query conn
	m_selected_conns.clear();
	QueryConnsByRect(sm::rect(pos, QUERY_REGION, QUERY_REGION), m_selected_conns);

	// query pin
	if (m_stage.GetSelection().Size() == 1)
	{
		n0::SceneNodePtr node = nullptr;
		m_stage.GetSelection().Traverse([&](const ee0::GameObjWithPos& nwp)->bool {
			node = nwp.GetNode();
			return false;
		});

		m_selected_pin = QueryPinsByPos(node, pos, m_first_pos);
		m_last_pos = m_first_pos;
	}

	return false;
}

bool ConnectPinsOP::OnMouseLeftUp(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftUp(x, y)) {
		return true;
	}

	if (m_selected_pin)
	{
		if (QueryOrCreateNode(x, y)) {
			m_stage.GetSubjectMgr()->NotifyObservers(MSG_BLUE_PRINT_CHANGED);
		}
		m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
		return true;
	}
	else
	{
		if (m_first_pos.IsValid())
		{
			auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
			sm::rect rect(m_first_pos, pos);
			rect.xmin -= QUERY_REGION;
			rect.ymin -= QUERY_REGION;
			rect.xmax += QUERY_REGION;
			rect.ymax += QUERY_REGION;

			m_selected_conns.clear();
			QueryConnsByRect(rect, m_selected_conns);
		}
	}

	return false;
}

bool ConnectPinsOP::OnMouseDrag(int x, int y)
{
	if (m_selected_pin)
	{
		m_last_pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);

		sm::vec2 v0, v3;
		if (m_selected_pin->IsInput()) {
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
	if (m_selected_pin)
	{
		if (m_first_pos.IsValid() && m_last_pos.IsValid())
		{
			pt2::PrimitiveDraw::SetColor(m_selected_pin->GetColor());
			pt2::PrimitiveDraw::Polyline(nullptr, m_curve.GetVertices(), false);
		}
	}
	else
	{
		if (ee0::EditOP::OnDraw()) {
			return true;
		}

		if (!m_selected_conns.empty())
		{
			pt2::PrimitiveDraw::SetColor(COL_SELECTED);
			for (auto& conn : m_selected_conns) {
				pt2::PrimitiveDraw::Polyline(
					nullptr, conn->GetCurve().shape.GetVertices(), false
				);
			}
		}
	}

	return false;
}

std::shared_ptr<Pins> ConnectPinsOP::QueryPinsByPos(const n0::SceneNodePtr& node,
	                                                const sm::vec2& pos, sm::vec2& p_center)
{
	if (!node->HasUniqueComp<CompNode>()) {
		return nullptr;
	}

	auto& ctrans = node->GetUniqueComp<n2::CompTransform>();
	auto& mat = ctrans.GetTrans().GetMatrix();

	auto& cnode = node->GetUniqueComp<CompNode>();
	auto& bp_node = cnode.GetNode();

	auto& input = bp_node->GetAllInput();
	for (auto& p : input)
	{
		auto center = NodeLayout::GetPinsPos(*p);
		if (sm::dis_pos_to_pos(pos, center) < NodeLayout::PINS_RADIUS * 1.5f)
		{
			p_center = center;
			return p;
		}
	}

	auto& output = bp_node->GetAllOutput();
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

void ConnectPinsOP::QueryConnsByRect(const sm::rect& rect, std::vector<std::shared_ptr<Connecting>>& conns)
{
	m_stage.Traverse([&](const ee0::GameObj& obj)->bool
	{
		if (!obj->HasUniqueComp<CompNode>()) {
			return true;
		}
		auto& cnode = obj->GetUniqueComp<CompNode>();
		auto bp_node = cnode.GetNode();
		if (!bp_node) {
			return true;
		}

		for (auto& pins : bp_node->GetAllOutput()) {
			for (auto& conn : pins->GetConnecting()) {
				auto& curve = conn->GetCurve();
				if (sm::is_rect_intersect_polyline(rect, curve.shape.GetVertices(), false)) {
					conns.push_back(conn);
				}
			}
		}

		return true;
	});
}

bool ConnectPinsOP::QueryOrCreateNode(int x, int y)
{
	bool dirty = false;

	std::shared_ptr<Pins> target = nullptr;
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
		if (m_selected_pin->CanTypeCast(target->GetType()) &&
			m_selected_pin->IsInput() != target->IsInput())
		{
			if (m_selected_pin->IsInput()) {
				make_connecting(target, m_selected_pin);
			} else {
				make_connecting(m_selected_pin, target);
			}
			dirty = true;
		}
	}
	else
	{
		dirty = CreateNode(x, y);
	}

	m_selected_pin = nullptr;
	m_curve.Clear();

	return dirty;
}

bool ConnectPinsOP::CreateNode(int x, int y)
{
	auto base = m_stage.GetScreenPosition();
	assert(m_selected_pin);
	WxCreateNodeDlg dlg(&m_stage, base + wxPoint(x, y), *m_selected_pin, m_nodes);
	if (dlg.ShowModal() != wxID_OK) {
		m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
		return false;
	}
	assert(m_selected_pin);

	auto type = dlg.GetSelectedType();
	NodePtr bp_node = NodeFactory::Instance()->Create(type);
	assert(bp_node);

	auto& style = bp_node->GetStyle();

	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	pos.x += style.width  * 0.5f;
	pos.y -= style.height * 0.5f;
	bp_node->SetPos(pos);

	auto builder = bp::NodeBuilder::Instance();
	builder->AfterCreated(*bp_node, m_stage.GetSubjectMgr());

	auto node = std::make_shared<n0::SceneNode>();
	auto& cnode = node->AddUniqueComp<bp::CompNode>();
	cnode.SetNode(bp_node);
	// trans
	auto& ctrans = node->AddUniqueComp<n2::CompTransform>();
	ctrans.SetPosition(*node, pos);
	// id
	node->AddUniqueComp<n0::CompIdentity>();
	// aabb
	node->AddUniqueComp<n2::CompBoundingBox>(
		sm::rect(style.width, style.height)
	);

	// connect
	auto pins_type = m_selected_pin->GetType();
	if (m_selected_pin->IsInput())
	{
		auto& output = bp_node->GetAllOutput();
		for (auto& pins : output) {
			if (pins->CanTypeCast(pins_type)) {
				make_connecting(pins, m_selected_pin);
				break;
			}
		}
	}
	else
	{
		auto& input = bp_node->GetAllInput();
		for (auto& pins : input) {
			if (pins->CanTypeCast(pins_type)) {
				make_connecting(m_selected_pin, pins);
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