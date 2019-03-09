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
		std::function<void(Node&, std::vector<n0::SceneNodePtr>&)> on_created = nullptr;
		std::function<void(Pins&, Pins&)> on_connecting = nullptr;
		std::function<void(Pins&, Pins&)> on_connected = nullptr;
        std::function<void(Pins&, Pins&)> on_disconnected = nullptr;
	};

	void RegistCB(const NodeBuilder::Callback& cb) { m_cb = cb; }

public:
	void OnCreated(Node& node, const ee0::SubjectMgrPtr& sub_mgr);

	void OnConnecting(Pins& from, Pins& to);
	void OnConnected(Pins& from, Pins& to);

    void OnDisconnected(Pins& from, Pins& to);

private:
	Callback m_cb;

	CU_SINGLETON_DECLARATION(NodeBuilder)

}; // NodeBuilder

}