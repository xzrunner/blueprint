#include "blueprint/Connecting.h"
#include "blueprint/Pins.h"
#include "blueprint/NodeLayout.h"

namespace
{

void deconnect(bp::node::Connecting& conn)
{
}

}

namespace bp
{
namespace node
{

Connecting::Connecting(const std::shared_ptr<Pins>& from, 
	                   const std::shared_ptr<Pins>& to)
	: m_from(from)
	, m_to(to)
{
	UpdateCurve();
}

void Connecting::UpdateCurve()
{
	sm::vec2 v0 = NodeLayout::GetPinsPos(*m_from);
	sm::vec2 v3 = NodeLayout::GetPinsPos(*m_to);
	float d = fabs((v3.x - v0.x) * NodeLayout::CONNECTING_BEZIER_DIST);
	auto v1 = v0 + sm::vec2(d, 0);
	auto v2 = v3 - sm::vec2(d, 0);
	m_curve.SetCtrlPos(v0, v1, v2, v3);
}

std::shared_ptr<Connecting> make_connecting(const std::shared_ptr<Pins>& from, 
	                                        const std::shared_ptr<Pins>& to)
{
	auto& to_conn = to->GetConnecting();
	if (!to_conn.empty()) 
	{
		assert(to_conn.size() == 1);
		auto& conn = to_conn[0];
		if (conn->GetFrom() == from &&
			conn->GetTo() == to) 
		{
			return conn;
		}
		else
		{
			conn->GetFrom()->RemoveConnecting(conn);
			conn->GetTo()->RemoveConnecting(conn);
		}
	}

	auto conn = std::make_shared<Connecting>(from, to);
	from->AddConnecting(conn);
	to->AddConnecting(conn);
	return conn;
}

}

}