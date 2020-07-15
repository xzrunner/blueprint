#pragma once

#include "blueprint/typedef.h"

#include <ee0/EditOP.h>

#include <SM_Vector.h>
#include <SM_Rect.h>
#include <node0/typedef.h>
#include <geoshape/Bezier.h>

//#define BP_CONNECT_PIN_OP_SELECT_CONNS

namespace ee0 { class WxStagePage; class AtomicOP; }

namespace bp
{

class Pin;
class Connecting;

class ConnectPinOP : public ee0::EditOP
{
public:
	ConnectPinOP(const std::shared_ptr<pt0::Camera>& cam, ee0::WxStagePage& stage,
		const std::vector<NodePtr>& nodes);

	virtual bool OnKeyDown(int key_code);

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseLeftUp(int x, int y) override;
    //virtual bool OnMouseRightDown(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;

	virtual bool OnDraw(const ur::Device& dev, ur::Context& ctx) const;

protected:
    virtual void BeforeConnect(const std::shared_ptr<Pin>& pin,
        const Node& bp_node, const n0::SceneNode& node) {}

private:
    void Connect(const std::shared_ptr<Pin>& pin, const Node& node);

	std::shared_ptr<Pin> QueryPinByPos(const n0::SceneNodePtr& node,
		const sm::vec2& pos, sm::vec2& p_center);
	void QueryConnsByRect(const sm::rect& rect, std::vector<std::shared_ptr<Connecting>>& conns);

	void QueryOrCreateNode(int x, int y, bool change_to);
    bool CreateNodeWithMousePos();
	bool CreateNode(int x, int y);

    void ClearSelectedConns();

    void PasteConnections();

    void MakeConnecting(const std::shared_ptr<Pin>& from, const std::shared_ptr<Pin>& to);
    void Disconnect(const std::shared_ptr<Connecting>& conn);

    void FlushRecords();

    enum class ConnEvent
    {
        Connecting,
        Connected,
    };
    void UpdateExtInputPorts(ConnEvent event);

    void GetAllSelected(std::vector<n0::SceneNodePtr>& selected);

protected:
    std::function<bool(const bp::Node&, const bp::Pin&)> m_is_port_matched = nullptr;

protected:
    ee0::WxStagePage& m_stage;

private:
	std::vector<NodePtr> m_nodes;

	std::shared_ptr<Pin> m_selected_pin = nullptr;
#ifdef BP_CONNECT_PIN_OP_SELECT_CONNS
	std::vector<std::shared_ptr<Connecting>> m_selected_conns;
#endif // BP_CONNECT_PIN_OP_SELECT_CONNS

	sm::vec2 m_first_pos;
	sm::vec2 m_last_pos;

	gs::Bezier m_curve;

    std::vector<std::shared_ptr<ee0::AtomicOP>> m_records;

    std::shared_ptr<Pin> m_last_selected_pin = nullptr;

}; // ConnectPinOP

}