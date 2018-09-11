#include "blueprint/NodeBuilder.h"

#include <ee0/MsgHelper.h>

namespace bp
{

CU_SINGLETON_DEFINITION(NodeBuilder);

NodeBuilder::NodeBuilder()
{
}

void NodeBuilder::AfterCreated(Node& node, const ee0::SubjectMgrPtr& sub_mgr)
{
	if (!m_cb.after_created) {
		return;
	}

	std::vector<n0::SceneNodePtr> nodes;
	m_cb.after_created(node, nodes);
	for (auto& n : nodes) {
		ee0::MsgHelper::InsertNode(*sub_mgr, n);
	}
}

void NodeBuilder::BeforeConnected(Pins& from, Pins& to)
{
	if (m_cb.before_connected) {
		m_cb.before_connected(from, to);
	}
}

void NodeBuilder::AfterConnected(Pins& from, Pins& to)
{
	if (m_cb.after_connected) {
		m_cb.after_connected(from, to);
	}
}

}