#include "blueprint/ConnectPinOP.h"
#include "blueprint/CompNode.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/RenderSystem.h"
#include "blueprint/Connecting.h"
#include "blueprint/WxCreateNodeDlg.h"
#include "blueprint/MessageID.h"
#include "blueprint/NodeBuilder.h"
#include "blueprint/ConnectPinAO.h"
#include "blueprint/DisconnectConnAO.h"
#include "blueprint/NodeStyle.h"
#include "blueprint/NSCompNode.h"

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
#include <unirender/RenderState.h>
#include <unirender/Factory.h>
#include <painting2/RenderSystem.h>
#include <tessellation/Painter.h>
#include <js/RapidJsonHelper.h>
#include <ns/NodeSerializer.h>

#include <queue>

namespace
{

const pt0::Color COL_SELECTED = pt0::Color(255, 0, 0);

const float QUERY_REGION = 5;

}

namespace bp
{

ConnectPinOP::ConnectPinOP(const std::shared_ptr<pt0::Camera>& cam, ee0::WxStagePage& stage,
	                         const std::vector<NodePtr>& nodes)
	: ee0::EditOP(cam)
	, m_stage(stage)
	, m_nodes(nodes)
{
	m_first_pos.MakeInvalid();
	m_last_pos.MakeInvalid();
}

bool ConnectPinOP::OnKeyDown(int key_code)
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

#ifdef BP_CONNECT_PIN_OP_SELECT_CONNS
    if (key_code == WXK_DELETE)
    {
        for (auto& conn : m_selected_conns) {
            Disconnect(conn);
        }
        m_selected_conns.clear();
    }
#endif // BP_CONNECT_PIN_OP_SELECT_CONNS

    // copy/paste connections
    //if (wxGetKeyState(WXK_CONTROL) && key_code == 'V') {
    //    PasteConnections();
    //}
    // fixme: WXK_CONTROL
    if (key_code == 'C')
    {
        std::vector<n0::SceneNodePtr> selected;
        GetAllSelected(selected);

        auto clipboard = ee0::Clipboard::Instance();
        rapidjson::Document doc;
        if (!doc.Parse<0>(clipboard->ToString().c_str()).HasParseError())
        {
            NSCompNode::StoreConnection(selected, doc, doc.GetAllocator());
            clipboard->FromString(js::RapidJsonHelper::ValueToString(doc));
        }
    }
    else if (key_code == 'V')
    {
//        PasteConnections();

        std::vector<n0::SceneNodePtr> selected;
        GetAllSelected(selected);

        auto str = ee0::Clipboard::Instance()->ToString();
        rapidjson::Document doc;
        if (!doc.Parse<0>(str.c_str()).HasParseError()) {
            bp::NSCompNode::LoadConnection(selected, doc);
        }
    }

	return false;
}

bool ConnectPinOP::OnMouseLeftDown(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftDown(x, y)) {
		return true;
	}

	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
    m_first_pos = pos;
	m_last_pos = m_first_pos;

#ifdef BP_CONNECT_PIN_OP_SELECT_CONNS
	// query conn
	m_selected_conns.clear();
	QueryConnsByRect(sm::rect(pos, QUERY_REGION, QUERY_REGION), m_selected_conns);
#endif // BP_CONNECT_PIN_OP_SELECT_CONNS

	// query pin
	if (m_stage.GetSelection().Size() == 1)
	{
		n0::SceneNodePtr node = nullptr;
		m_stage.GetSelection().Traverse([&](const ee0::GameObjWithPos& nwp)->bool {
			node = nwp.GetNode();
			return false;
		});

		m_selected_pin = QueryPinByPos(node, pos, m_first_pos);
		m_last_pos = m_first_pos;

        m_last_selected_pin = m_selected_pin;
        UpdateExtInputPorts(ConnEvent::Connecting);
	}

	return false;
}

bool ConnectPinOP::OnMouseLeftUp(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftUp(x, y)) {
		return true;
	}

    bool ret = false;
	if (m_selected_pin)
	{
        bool change_to = m_selected_pin->IsInput() && !m_selected_pin->GetConnecting().empty();
        QueryOrCreateNode(x, y, change_to);
        m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
		m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
#ifdef BP_CONNECT_PIN_OP_SELECT_CONNS
        m_selected_conns.clear();
#endif // BP_CONNECT_PIN_OP_SELECT_CONNS

        UpdateExtInputPorts(ConnEvent::Connected);

        // clear
        m_selected_pin = nullptr;
        m_curve.SetCtrlPos({});

        ret = true;
	}
#ifdef BP_CONNECT_PIN_OP_SELECT_CONNS
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
#endif // BP_CONNECT_PIN_OP_SELECT_CONNS

    m_first_pos.MakeInvalid();
    m_last_pos.MakeInvalid();

    m_last_selected_pin.reset();

//    FlushRecords();

	return ret;
}

//bool ConnectPinOP::OnMouseRightDown(int x, int y)
//{
//    if (ee0::EditOP::OnMouseRightDown(x, y)) {
//        return true;
//    }
//
//    m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
//
//    return false;
//}

bool ConnectPinOP::OnMouseDrag(int x, int y)
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
                v3 = NodeLayout::GetPinPos(*conns[0]->GetFrom());
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

bool ConnectPinOP::OnDraw(const ur::Device& dev, ur::Context& ctx) const
{
	if (m_selected_pin)
	{
		if (m_first_pos.IsValid() && m_last_pos.IsValid())
		{
			tess::Painter pt;
			pt2::RenderSystem::DrawShape(pt, m_curve, m_selected_pin->GetColor().ToABGR());

			pt2::RenderSystem::DrawPainter(dev, ctx, ur::DefaultRenderState2D(), pt);
		}
	}
	else
	{
		if (ee0::EditOP::OnDraw(dev, ctx)) {
			return true;
		}

#ifdef BP_CONNECT_PIN_OP_SELECT_CONNS
		if (!m_selected_conns.empty())
		{
			tess::Painter pt;
			auto col = COL_SELECTED.ToABGR();
			for (auto& conn : m_selected_conns) {
				pt2::RenderSystem::DrawShape(pt, conn->GetCurve().shape, col);
			}
			pt2::RenderSystem::DrawPainter(pt);
		}
#endif // BP_CONNECT_PIN_OP_SELECT_CONNS
	}

	return false;
}

void ConnectPinOP::Connect(const std::shared_ptr<Pin>& pin, const Node& node)
{
	if (pin->IsInput())
	{
		auto& output = node.GetAllOutput();
		for (auto& o : output) {
			if (o->CanTypeCast(*pin)) {
				MakeConnecting(o, pin);
				break;
			}
		}
	}
	else
	{
		auto& input = node.GetAllInput();
		for (auto& i : input) {
			if (i->CanTypeCast(*pin)) {
				MakeConnecting(pin, i);
				break;
			}
		}
	}
}

std::shared_ptr<Pin> ConnectPinOP::QueryPinByPos(const n0::SceneNodePtr& node,
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
		auto center = NodeLayout::GetPinPos(*p);
		if (sm::dis_pos_to_pos(pos, center) < NodeLayout::PIN_RADIUS * 1.5f)
		{
			p_center = center;
			return p;
		}
	}

	auto& output = bp_node->GetAllOutput();
	for (auto& p : output)
	{
		auto center = NodeLayout::GetPinPos(*p);
		if (sm::dis_pos_to_pos(pos, center) < NodeLayout::PIN_RADIUS * 1.5f) {
			p_center = center;
			return p;
		}
	}

	return nullptr;
}

void ConnectPinOP::QueryConnsByRect(const sm::rect& rect, std::vector<std::shared_ptr<Connecting>>& conns)
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

		for (auto& pin : bp_node->GetAllOutput()) {
			for (auto& conn : pin->GetConnecting()) {
				auto& curve = conn->GetCurve();
				if (sm::is_rect_intersect_polyline(rect, curve.shape.GetVertices(), false)) {
					conns.push_back(conn);
				}
			}
		}

		return true;
	});
}

void ConnectPinOP::QueryOrCreateNode(int x, int y, bool change_to)
{
	std::shared_ptr<Pin> target = nullptr;
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	m_stage.Traverse([&](const ee0::GameObj& obj)->bool
	{
		auto& cbb = obj->GetUniqueComp<n2::CompBoundingBox>();
		if (!cbb.GetBounding(*obj).IsContain(pos)) {
			return true;
		}

		sm::vec2 center;
		target = QueryPinByPos(obj, pos, center);
		return !target;
	});
    if (change_to)
    {
        if (target != m_selected_pin)
        {
            auto& conns = m_selected_pin->GetConnecting();
            assert(conns.size() == 1);
            auto from = conns[0]->GetFrom();
            if (target && target->IsInput())
            {
                assert(!from->IsInput());
                if (from->CanTypeCast(*target))
                {
                    Disconnect(conns[0]);
                    MakeConnecting(from, target);
                }
            }
            else
            {
                Disconnect(conns[0]);
            }
        }
    }
    else
    {
	    if (target)
	    {
		    if (m_selected_pin->CanTypeCast(*target) &&
			    m_selected_pin->IsInput() != target->IsInput())
		    {
                std::shared_ptr<Pin> from, to;
			    if (m_selected_pin->IsInput()) {
                    from = target;
                    to = m_selected_pin;
			    } else {
                    from = m_selected_pin;
                    to = target;
			    }

                auto& conns = to->GetConnecting();
                if (!conns.empty()) {
                    assert(conns.size() == 1);
                    Disconnect(conns[0]);
                }

                MakeConnecting(from, to);
		    }
	    }
	    else
	    {
            CreateNode(x, y);
	    }
    }

    // clear
    m_selected_pin = nullptr;
    m_curve.SetCtrlPos({});
}

bool ConnectPinOP::CreateNodeWithMousePos()
{
    wxPoint pos = wxGetMousePosition();
    const wxPoint pt = wxGetMousePosition();
    int mouse_x = pt.x - m_stage.GetScreenPosition().x;
    int mouse_y = pt.y - m_stage.GetScreenPosition().y;
    return CreateNode(mouse_x, mouse_y);
}

bool ConnectPinOP::CreateNode(int x, int y)
{
	auto base = m_stage.GetScreenPosition();
	WxCreateNodeDlg dlg(&m_stage, base + wxPoint(x, y), m_selected_pin, m_nodes, m_is_port_matched);
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

    // insert
    ee0::MsgHelper::InsertNode(*m_stage.GetSubjectMgr(), node);

	// connect
    if (m_selected_pin)
    {
        BeforeConnect(m_selected_pin, *bp_node, *node);
        Connect(m_selected_pin, *bp_node);
        node->GetUniqueComp<n2::CompBoundingBox>().SetSize(
            *node, sm::rect(style.width, style.height)
        );
    }

    m_stage.GetSubjectMgr()->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);

	return true;
}

void ConnectPinOP::ClearSelectedConns()
{
    m_stage.GetSelection().Traverse([&](const ee0::GameObjWithPos& nwp)->bool
    {
        auto node = nwp.GetNode()->GetUniqueComp<CompNode>().GetNode();
        for (auto& port : node->GetAllInput()) {
            auto conns = port->GetConnecting();
            for (auto& conn : conns) {
                Disconnect(conn);
            }
        }
        for (auto& port : node->GetAllOutput()) {
            auto conns = port->GetConnecting();
            for (auto& conn : conns) {
                Disconnect(conn);
            }
        }
        return true;
    });
}

void ConnectPinOP::PasteConnections()
{
    //auto& cb = ee0::Clipboard::Instance()->GetSceneNodes();

    //std::vector<n0::SceneNodePtr> new_cb;
    //new_cb.reserve(m_stage.GetSelection().Size());
    //m_stage.GetSelection().Traverse([&](const ee0::GameObjWithPos& owp)->bool
    //{
    //    new_cb.push_back(owp.GetNode());
    //    return true;
    //});

    //// check
    //assert(cb.size() == new_cb.size());
    //for (int i = 0, n = cb.size(); i < n; ++i)
    //{
    //    assert(cb[i]->HasUniqueComp<CompNode>()
    //        && new_cb[i]->HasUniqueComp<CompNode>());
    //    auto& old_cnode = cb[i]->GetUniqueComp<CompNode>();
    //    auto& new_cnode = new_cb[i]->GetUniqueComp<CompNode>();
    //    assert(old_cnode.GetNode()->get_type() == new_cnode.GetNode()->get_type());
    //}

    //// prepare lut
    //std::map<const Node*, int> map_node2idx;
    //for (int i = 0, n = cb.size(); i < n; ++i) {
    //    auto& cnode = cb[i]->GetUniqueComp<CompNode>();
    //    map_node2idx.insert({ cnode.GetNode().get(), i });
    //}

    //// connect
    //for (int i = 0, n = cb.size(); i < n; ++i)
    //{
    //    auto& out_pins = cb[i]->GetUniqueComp<CompNode>().GetNode()->GetAllOutput();
    //    for (int j = 0, m = out_pins.size(); j < m; ++j)
    //    {
    //        auto& conns = out_pins[j]->GetConnecting();
    //        for (int k = 0, l = conns.size(); k < l; ++k)
    //        {
    //            auto to_pin = conns[k]->GetTo();
    //            auto itr = map_node2idx.find(&to_pin->GetParent());
    //            if (itr == map_node2idx.end()) {
    //                continue;
    //            }

    //            auto pin_idx = to_pin->GetPosIdx();
    //            auto node_idx = itr->second;
    //            auto from = new_cb[i]->GetUniqueComp<CompNode>().GetNode()->GetAllOutput()[j];
    //            auto to = new_cb[node_idx]->GetUniqueComp<CompNode>().GetNode()->GetAllInput()[pin_idx];
    //            MakeConnecting(from, to);
    //        }
    //    }
    //}
}

void ConnectPinOP::MakeConnecting(const std::shared_ptr<Pin>& from, const std::shared_ptr<Pin>& to)
{
    //auto sub_mgr = m_stage.GetSubjectMgr();
    //auto aop = std::make_shared<ConnectPinAO>(sub_mgr, from, to);
    //m_records.push_back(aop);

    auto conn = make_connecting(from, to);

    ee0::VariantSet vars;
    ee0::Variant var;
    var.m_type = ee0::VT_PVOID;
    var.m_val.pv = &conn;
    vars.SetVariant("conn", var);
    m_stage.GetSubjectMgr()->NotifyObservers(MSG_BP_CONN_INSERT, vars);
}

void ConnectPinOP::Disconnect(const std::shared_ptr<Connecting>& conn)
{
    //auto sub_mgr = m_stage.GetSubjectMgr();
    //auto aop = std::make_shared<DisconnectConnAO>(sub_mgr, conn->GetFrom(), conn->GetTo());
    //m_records.push_back(aop);

    ee0::VariantSet vars;
    ee0::Variant var;
    var.m_type = ee0::VT_PVOID;
    var.m_val.pv = &conn;
    vars.SetVariant("conn", var);
    m_stage.GetSubjectMgr()->NotifyObservers(MSG_BP_BEFORE_CONN_DELETE, vars);

    disconnect(conn);

    m_stage.GetSubjectMgr()->NotifyObservers(MSG_BP_AFTER_CONN_DELETE, vars);
}

void ConnectPinOP::FlushRecords()
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

void ConnectPinOP::UpdateExtInputPorts(ConnEvent event)
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

void ConnectPinOP::GetAllSelected(std::vector<n0::SceneNodePtr>& selected)
{
    selected.reserve(m_stage.GetSelection().Size());
    m_stage.GetSelection().Traverse([&](const ee0::GameObjWithPos& owp)->bool
    {
        selected.push_back(owp.GetNode());
        return true;
    });
}

}