#pragma once

#include <ee0/typedef.h>

#include <cu/cu_macro.h>
#include <node0/typedef.h>

#include <functional>
#include <vector>

namespace bp
{

class Node;
class Pin;

class NodeBuilder
{
public:
	struct Callback
	{
		std::function<void(Node&, std::vector<n0::SceneNodePtr>&)> on_created = nullptr;
		std::function<void(Pin&, Pin&)> on_connecting = nullptr;
		std::function<void(Pin&, Pin&)> on_connected = nullptr;
        std::function<void(Pin&, Pin&)> on_disconnected = nullptr;
	};

	void RegistCB(const NodeBuilder::Callback& cb) { m_cb = cb; }

public:
	void OnCreated(Node& node, const ee0::SubjectMgrPtr& sub_mgr);

	void OnConnecting(Pin& from, Pin& to);
	void OnConnected(Pin& from, Pin& to);

    void OnDisconnected(Pin& from, Pin& to);

private:
	Callback m_cb;

	CU_SINGLETON_DECLARATION(NodeBuilder)

}; // NodeBuilder

}