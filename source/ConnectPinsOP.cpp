#include "blueprint/ConnectPinsOP.h"
#include "blueprint/CompNode.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/RenderSystem.h"
#include "blueprint/Connecting.h"
#include "blueprint/WxCreateNodeDlg.h"
#include "blueprint/MessageID.h"
#include "blueprint/NodeBuilder.h"
#include "blueprint/ConnectPinsAO.h"
#include "blueprint/DisconnectConnAO.h"
#include "blueprint/NodeStyle.h"

#include <ee0/WxStagePage.h>
#include <ee0/CameraHelper.h>
#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>
#include <ee0/MsgHelper.h>
#include <ee0/CombineAOP.h>
#include <ee0/Clipboard.h>

#include <SM_Calc.h>
#include <node0/SceneNode.h>
#include <node0/CompIdentity.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <painting2/RenderSystem.h>
#include <tessellation/Painter.h>

#include <queue>

namespace
{

const pt0::Color COL_SELECTED = pt0::Color(255, 0, 0);

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
    // create new node
    if (key_code == WXK_SPACE) {
        CreateNodeWithMousePos();
        return true;
    }

    // delete connections
    if (key_code == WXK_DELETE) {
        ClearSelectedConns();
    }

    if (ee0::EditOP::OnKeyDown(key_code)) {
        return true;
    }

#ifdef BP_CONNECT_PINS_OP_SELECT_CONNS
    if (key_code == WXK_DELETE)
    {
        for (auto& conn : m_selected_conns) {
            Disconnect(conn);
        }
        m_selected_conns.clear();
    }
#endif // BP_CONNECT_PINS_OP_SELECT_CONNS

    // copy/paste connections
    if (wxGetKeyState(WXK_CONTROL) && key_code == 'V') {
        PasteConnections();
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

#ifdef BP_CONNECT_PINS_OP_SELECT_CONNS
	// query conn
	m_selected_conns.clear();
	QueryConnsByRect(sm::rect(pos, QUERY_REGION, QUERY_REGION), m_selected_conns);
#endif // BP_CONNECT_PINS_OP_SELECT_CONNS

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

        m_last_selected_pin = m_selected_pin;
        UpdatemExtInputPorts(ConnEvent::Connecting);
	}

	return false;
}

bool ConnectPinsOP::OnMouseLeftUp(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftUp(x, y)) {
		return true;
	}

    bool ret = false;
	if (m_selected_pin)
	{
        bool change_to = m_selected_pin->IsInput() && !m_selected_pin->GetConnecting().empty();
        if (QueryOrCreateNode(x, y, change_to)) {
            m_stage.GetSubjectMgr()->NotifyObservers(MSG_BLUE_PRINT_CHANGED);
        }
        m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
		m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
#ifdef BP_CONNECT_PINS_OP_SELECT_CONNS
        m_selected_conns.clear();
#endif // BP_CONNECT_PINS_OP_SELECT_CONNS

        UpdatemExtInputPorts(ConnEvent::Connected);

        // clear
        m_selected_pin = nullptr;
        m_curve.SetCtrlPos({});

        ret = true;
	}
#ifdef BP_CONNECT_PINS_OP_SELECT_CONNS
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
#endif // BP_CONNECT_PINS_OP_SELECT_CONNS

    m_first_pos.MakeInvalid();
    m_last_pos.MakeInvalid();

    m_last_selected_pin.reset();

//    FlushRecords();

	return ret;
}

bool ConnectPinsOP::OnMouseRightDown(int x, int y)
{
    if (ee0::EditOP::OnMouseRightDown(x, y)) {
        return true;
    }

    m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);

    return false;
}

bool ConnectPinsOP::OnMouseDrag(int x, int y)
{
	if (m_selected_pin)
	{
		m_last_pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);

		sm::vec2 v0, v3;
		if (m_selected_pin->IsInput())
        {
            if (!m_selected_pin->GetConnecting().empty())
            {
                v0 = m_last_pos;
                auto& conns = m_selected_pin->GetConnecting();
                assert(conns.size() == 1);
                v3 = NodeLayout::GetPinsPos(*conns[0]->GetFrom());
            }
            else
            {
                v0 = m_last_pos;
                v3 = m_first_pos;
            }
		}
        else
        {
			v0 = m_first_pos;
			v3 = m_last_pos;
		}

		float d = fabs((v3.x - v0.x) * NodeLayout::CONNECTING_BEZIER_DIST);
		auto v1 = v0 + sm::vec2(d, 0);
		auto v2 = v3 - sm::vec2(d, 0);
		m_curve.SetCtrlPos({ v0, v1, v2, v3 });

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
			tess::Painter pt;
			pt2::RenderSystem::DrawShape(pt, m_curve, m_selected_pin->GetColor().ToABGR());
			pt2::RenderSystem::DrawPainter(pt);
		}
	}
	else
	{
		if (ee0::EditOP::OnDraw()) {
			return true;
		}

#ifdef BP_CONNECT_PINS_OP_SELECT_CONNS
		if (!m_selected_conns.empty())
		{
			tess::Painter pt;
			auto col = COL_SELECTED.ToABGR();
			for (auto& conn : m_selected_conns) {
				pt2::RenderSystem::DrawShape(pt, conn->GetCurve().shape, col);
			}
			pt2::RenderSystem::DrawPainter(pt);
		}
#endif // BP_CONNECT_PINS_OP_SELECT_CONNS
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

bool ConnectPinsOP::QueryOrCreateNode(int x, int y, bool change_to)
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
    if (change_to)
    {
        if (target != m_selected_pin)
        {
            auto& conns = m_selected_pin->GetConnecting();
            assert(conns.size() == 1);
            auto from = conns[0]->GetFrom();
            Disconnect(conns[0]);
            if (target && target->IsInput()) {
                MakeConnecting(from, target);
            }
            dirty = true;
        }
    }
    else
    {
	    if (target)
	    {
		    if (m_selected_pin->CanTypeCast(*target) &&
			    m_selected_pin->IsInput() != target->IsInput())
		    {
			    if (m_selected_pin->IsInput()) {
				    MakeConnecting(target, m_selected_pin);
			    } else {
				    MakeConnecting(m_selected_pin, target);
			    }
			    dirty = true;
		    }
	    }
	    else
	    {
            dirty = CreateNode(x, y);
	    }
    }

    // clear
    m_selected_pin = nullptr;
    m_curve.SetCtrlPos({});

	return dirty;
}

bool ConnectPinsOP::CreateNodeWithMousePos()
{
    wxPoint pos = wxGetMousePosition();
    const wxPoint pt = wxGetMousePosition();
    int mouse_x = pt.x - m_stage.GetScreenPosition().x;
    int mouse_y = pt.y - m_stage.GetScreenPosition().y;
    return CreateNode(mouse_x, mouse_y);
}

bool ConnectPinsOP::CreateNode(int x, int y)
{
	auto base = m_stage.GetScreenPosition();
	WxCreateNodeDlg dlg(&m_stage, base + wxPoint(x, y), m_selected_pin, m_nodes);
	if (dlg.ShowModal() != wxID_OK) {
		m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
		return false;
	}

	auto type = dlg.GetSelectedType();
	auto rt_type = rttr::type::get_by_name(type);
	assert(rt_type.is_valid());
	auto rt_obj = rt_type.create();
	assert(rt_obj.is_valid());
	NodePtr bp_node = rt_obj.get_value<NodePtr>();
	assert(bp_node);

	auto& style = bp_node->GetStyle();

	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	pos.x += style.width  * 0.5f;
	pos.y -= style.height * 0.5f;
	bp_node->SetPos(pos);

	auto builder = NodeBuilder::Instance();
	builder->OnCreated(*bp_node, m_stage.GetSubjectMgr());

	auto node = std::make_shared<n0::SceneNode>();
	auto& cnode = node->AddUniqueComp<CompNode>();
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
    if (m_selected_pin)
    {
	    if (m_selected_pin->IsInput())
	    {
		    auto& output = bp_node->GetAllOutput();
		    for (auto& pins : output) {
			    if (pins->CanTypeCast(*m_selected_pin)) {
				    MakeConnecting(pins, m_selected_pin);
				    break;
			    }
		    }
	    }
	    else
	    {
		    auto& input = bp_node->GetAllInput();
		    for (auto& pins : input) {
			    if (pins->CanTypeCast(*m_selected_pin)) {
				    MakeConnecting(m_selected_pin, pins);
				    break;
			    }
		    }
	    }
    }

    ee0::MsgHelper::InsertNode(*m_stage.GetSubjectMgr(), node);

    m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);

	return true;
}

void ConnectPinsOP::ClearSelectedConns()
{
    m_stage.GetSelection().Traverse([&](const ee0::GameObjWithPos& nwp)->bool
    {
        auto node = nwp.GetNode()->GetUniqueComp<CompNode>().GetNode();
        for (auto& port : node->GetAllInput()) {
            for (auto& conn : port->GetConnecting()) {
                Disconnect(conn);
            }
        }
        for (auto& port : node->GetAllOutput()) {
            for (auto& conn : port->GetConnecting()) {
                Disconnect(conn);
            }
        }
        return true;
    });
}

void ConnectPinsOP::PasteConnections()
{
    auto& cb = ee0::Clipboard::Instance()->GetSceneNodes();

    std::vector<n0::SceneNodePtr> new_cb;
    new_cb.reserve(m_stage.GetSelection().Size());
    m_stage.GetSelection().Traverse([&](const ee0::GameObjWithPos& owp)->bool
    {
        new_cb.push_back(owp.GetNode());
        return true;
    });

    // check
    assert(cb.size() == new_cb.size());
    for (int i = 0, n = cb.size(); i < n; ++i)
    {
        assert(cb[i]->HasUniqueComp<CompNode>()
            && new_cb[i]->HasUniqueComp<CompNode>());
        auto& old_cnode = cb[i]->GetUniqueComp<CompNode>();
        auto& new_cnode = new_cb[i]->GetUniqueComp<CompNode>();
        assert(old_cnode.GetNode()->get_type() == new_cnode.GetNode()->get_type());
    }

    // prepare lut
    std::map<const Node*, int> map_node2idx;
    for (int i = 0, n = cb.size(); i < n; ++i) {
        auto& cnode = cb[i]->GetUniqueComp<CompNode>();
        map_node2idx.insert({ cnode.GetNode().get(), i });
    }

    // connect
    for (int i = 0, n = cb.size(); i < n; ++i)
    {
        auto& out_pins = cb[i]->GetUniqueComp<CompNode>().GetNode()->GetAllOutput();
        for (int j = 0, m = out_pins.size(); j < m; ++j)
        {
            auto& conns = out_pins[j]->GetConnecting();
            for (int k = 0, l = conns.size(); k < l; ++k)
            {
                auto to_pins = conns[k]->GetTo();
                auto itr = map_node2idx.find(&to_pins->GetParent());
                if (itr == map_node2idx.end()) {
                    continue;
                }

                auto pins_idx = to_pins->GetPosIdx();
                auto node_idx = itr->second;
                auto from = new_cb[i]->GetUniqueComp<CompNode>().GetNode()->GetAllOutput()[j];
                auto to = new_cb[node_idx]->GetUniqueComp<CompNode>().GetNode()->GetAllInput()[pins_idx];
                MakeConnecting(from, to);
            }
        }
    }
}

void ConnectPinsOP::MakeConnecting(const std::shared_ptr<Pins>& from, const std::shared_ptr<Pins>& to)
{
    //auto sub_mgr = m_stage.GetSubjectMgr();
    //auto aop = std::make_shared<ConnectPinsAO>(sub_mgr, from, to);
    //m_records.push_back(aop);

    make_connecting(from, to);
}

void ConnectPinsOP::Disconnect(const std::shared_ptr<Connecting>& conn)
{
    //auto sub_mgr = m_stage.GetSubjectMgr();
    //auto aop = std::make_shared<DisconnectConnAO>(sub_mgr, conn->GetFrom(), conn->GetTo());
    //m_records.push_back(aop);

    disconnect(conn);
}

void ConnectPinsOP::FlushRecords()
{
    if (m_records.empty()) {
        return;
    }

    auto sub_mgr = m_stage.GetSubjectMgr();
    if (m_records.size() == 1)
    {
        ee0::MsgHelper::AddAtomicOP(*sub_mgr, m_records[0]);
    }
    else
    {
        auto aop = std::make_shared<ee0::CombineAOP>();
        for (auto& r : m_records) {
            aop->Insert(r);
        }
        ee0::MsgHelper::AddAtomicOP(*sub_mgr, aop);
    }
    m_records.clear();
}

void ConnectPinsOP::UpdatemExtInputPorts(ConnEvent event)
{
    if (!m_last_selected_pin) {
        return;
    }

    bool is_connecting = event == ConnEvent::Connecting;
 	m_stage.Traverse([&](const ee0::GameObj& obj)->bool
	{
		if (!obj->HasUniqueComp<CompNode>()) {
			return true;
		}
		auto& cnode = obj->GetUniqueComp<CompNode>();
		auto bp_node = cnode.GetNode();
		if (bp_node && bp_node->IsExtensibleInputPorts())
        {
            if (is_connecting && bp_node.get() == &m_last_selected_pin->GetParent()) {
                return true;
            }

            if (bp_node->UpdateExtInputPorts(is_connecting))
            {
                auto& st = bp_node->GetStyle();
                sm::vec2 old_sz(st.width, st.height);

                NodeLayout::UpdateNodeStyle(*bp_node);
                sm::vec2 new_sz(st.width, st.height);

                auto& caabb = obj->GetUniqueComp<n2::CompBoundingBox>();
                caabb.SetSize(*obj, sm::rect(st.width, st.height));

                auto& ctrans = obj->GetUniqueComp<n2::CompTransform>();
                auto pos = ctrans.GetTrans().GetPosition();
                pos.x += (new_sz.x - old_sz.x) * 0.5f;
                pos.y -= (new_sz.y - old_sz.y) * 0.5f;
                ctrans.SetPosition(*obj, pos);
            }
		}

		return true;
	});
}

}