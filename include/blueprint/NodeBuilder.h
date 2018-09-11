#pragma once

#include <ee0/typedef.h>

#include <cu/cu_macro.h>
#include <node0/typedef.h>

#include <functional>
#include <vector>

namespace bp
{

class Node;
class Pins;

class NodeBuilder
{
public:
	struct Callback
	{
		std::function<void(Node&, std::vector<n0::SceneNodePtr>&)> after_created = nullptr;
		std::function<void(Pins&, Pins&)>                          before_connected = nullptr;
		std::function<void(Pins&, Pins&)>                          after_connected = nullptr;
	};

	void RegistCB(const NodeBuilder::Callback& cb) { m_cb = cb; }

public:
	void AfterCreated(Node& node, const ee0::SubjectMgrPtr& sub_mgr);

	void BeforeConnected(Pins& from, Pins& to);
	void AfterConnected(Pins& from, Pins& to);

private:
	Callback m_cb;

	CU_SINGLETON_DECLARATION(NodeBuilder)

}; // NodeBuilder

}