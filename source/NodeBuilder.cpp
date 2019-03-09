#include "blueprint/NodeBuilder.h"

#include <ee0/MsgHelper.h>

namespace bp
{

CU_SINGLETON_DEFINITION(NodeBuilder);

NodeBuilder::NodeBuilder()
{
}

void NodeBuilder::OnCreated(Node& node, const ee0::SubjectMgrPtr& sub_mgr)
{
	if (!m_cb.on_created) {
		return;
	}

	std::vector<n0::SceneNodePtr> nodes;
	m_cb.on_created(node, nodes);
	for (auto& n : nodes) {
		ee0::MsgHelper::InsertNode(*sub_mgr, n);
	}
}

void NodeBuilder::OnConnecting(Pins& from, Pins& to)
{
	if (m_cb.on_connecting) {
		m_cb.on_connecting(from, to);
	}
}

void NodeBuilder::OnConnected(Pins& from, Pins& to)
{
	if (m_cb.on_connected) {
		m_cb.on_connected(from, to);
	}
}

void NodeBuilder::OnDisconnected(Pins& from, Pins& to)
{
    if (m_cb.on_disconnected) {
        m_cb.on_disconnected(from, to);
    }
}

}