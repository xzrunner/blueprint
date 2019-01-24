#include "blueprint/ConnectPinsOP.h"
#include "blueprint/CompNode.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/RenderSystem.h"
#include "blueprint/Connecting.h"
#include "blueprint/WxCreateNodeDlg.h"
#include "blueprint/MessageID.h"
#include "blueprint/NodeBuilder.h"

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
#include <painting2/RenderSystem.h>
#include <tessellation/Painter.h>

#include <queue>

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
            disconnect(conn);
        }
        m_selected_conns.clear();
    }
#endif // BP_CONNECT_PINS_OP_SELECT_CONNS

    // copy/paste connections
    if (wxGetKeyState(WXK_CONTROL)) {
        if (key_code == 'C') {
            CopyConnections();
        } else if (key_code == 'V') {
            PasteConnections();
        }
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

bool ConnectPinsOP::OnMouseLeftDClick(int x, int y)
{
    if (ee0::EditOP::OnMouseLeftDClick(x, y)) {
        return true;
    }

    // select tree
    if (m_stage.GetSelection().Size() == 1)
    {
        NodePtr root = nullptr;
        m_stage.GetSelection().Traverse([&](const ee0::GameObjWithPos& nwp)->bool
        {
            auto& node = nwp.GetNode();
            assert(node->HasUniqueComp<CompNode>());
            root = node->GetUniqueComp<bp::CompNode>().GetNode();
            return true;
        });
        SelectAllTree(root);
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
            disconnect(conns[0]);
            if (target && target->IsInput()) {
                make_connecting(from, target);
            }
            dirty = true;
        }
    }
    else
    {
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
    if (m_selected_pin)
    {
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
    }

    ee0::MsgHelper::InsertNode(*m_stage.GetSubjectMgr(), node);

    m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);

	return true;
}

void ConnectPinsOP::ClearSelectedConns()
{
    m_stage.GetSelection().Traverse([](const ee0::GameObjWithPos& nwp)->bool
    {
        auto node = nwp.GetNode()->GetUniqueComp<bp::CompNode>().GetNode();
        for (auto& port : node->GetAllInput()) {
            for (auto& conn : port->GetConnecting()) {
                disconnect(conn);
            }
        }
        return true;
    });
}

void ConnectPinsOP::CopyConnections()
{
    m_clipboard.clear();
    m_clipboard.reserve(m_stage.GetSelection().Size());
    m_stage.GetSelection().Traverse([&](const ee0::GameObjWithPos& owp)->bool
    {
        m_clipboard.push_back(owp.GetNode());
        return true;
    });
}

void ConnectPinsOP::PasteConnections()
{
    std::vector<n0::SceneNodePtr> new_cb;
    m_clipboard.reserve(m_stage.GetSelection().Size());
    m_stage.GetSelection().Traverse([&](const ee0::GameObjWithPos& owp)->bool
    {
        new_cb.push_back(owp.GetNode());
        return true;
    });

    // check
    assert(m_clipboard.size() == new_cb.size());
    for (int i = 0, n = m_clipboard.size(); i < n; ++i)
    {
        assert(m_clipboard[i]->HasUniqueComp<bp::CompNode>()
            && new_cb[i]->HasUniqueComp<bp::CompNode>());
        auto& old_cnode = m_clipboard[i]->GetUniqueComp<bp::CompNode>();
        auto& new_cnode = new_cb[i]->GetUniqueComp<bp::CompNode>();
        assert(old_cnode.GetNode()->get_type() == new_cnode.GetNode()->get_type());
    }

    // prepare lut
    std::map<const bp::Node*, int> map_node2idx;
    for (int i = 0, n = m_clipboard.size(); i < n; ++i) {
        auto& cnode = m_clipboard[i]->GetUniqueComp<bp::CompNode>();
        map_node2idx.insert({ cnode.GetNode().get(), i });
    }

    // connect
    for (int i = 0, n = m_clipboard.size(); i < n; ++i)
    {
        auto& out_pins = m_clipboard[i]->GetUniqueComp<bp::CompNode>().GetNode()->GetAllOutput();
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
                auto from = new_cb[i]->GetUniqueComp<bp::CompNode>().GetNode()->GetAllOutput()[j];
                auto to = new_cb[node_idx]->GetUniqueComp<bp::CompNode>().GetNode()->GetAllInput()[pins_idx];
                bp::make_connecting(from, to);
            }
        }
    }
}

void ConnectPinsOP::SelectAllTree(const NodePtr& root) const
{
    std::set<const bp::Node*> sel_bp_nodes;

    std::queue<const bp::Node*> buf;
    buf.push(root.get());
    while (!buf.empty())
    {
        auto n = buf.front(); buf.pop();
        sel_bp_nodes.insert(n);

        for (auto& out : n->GetAllOutput()) {
            for (auto& conn : out->GetConnecting()) {
                buf.push(&conn->GetTo()->GetParent());
            }
        }
    }

    if (sel_bp_nodes.empty()) {
        return;
    }

    std::vector<n0::SceneNodePtr> sel_nodes;
    m_stage.Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (obj->HasUniqueComp<bp::CompNode>())
        {
            auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
            if (sel_bp_nodes.find(bp_node.get()) != sel_bp_nodes.end()) {
                sel_nodes.push_back(obj);
            }
        }
        return true;
    });
    auto sub_mgr = m_stage.GetSubjectMgr();

    std::vector<n0::NodeWithPos> nwps;
    nwps.reserve(sel_nodes.size());
    for (auto& obj : sel_nodes) {
        nwps.push_back(n0::NodeWithPos(obj, obj, 0));
    }
    ee0::MsgHelper::InsertSelection(*sub_mgr, nwps);
}

}