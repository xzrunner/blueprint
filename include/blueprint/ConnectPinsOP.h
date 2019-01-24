#pragma once

#include "blueprint/typedef.h"

#include <ee0/EditOP.h>

#include <SM_Vector.h>
#include <SM_Rect.h>
#include <node0/typedef.h>
#include <geoshape/Bezier.h>

//#define BP_CONNECT_PINS_OP_SELECT_CONNS

namespace ee0 { class WxStagePage; }

namespace bp
{

class Pins;
class Connecting;

class ConnectPinsOP : public ee0::EditOP
{
public:
	ConnectPinsOP(const std::shared_ptr<pt0::Camera>& cam, ee0::WxStagePage& stage,
		const std::vector<NodePtr>& nodes);

	virtual bool OnKeyDown(int key_code);

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseLeftUp(int x, int y) override;
    virtual bool OnMouseRightDown(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;
    virtual bool OnMouseLeftDClick(int x, int y) override;

	virtual bool OnDraw() const;

private:
	std::shared_ptr<Pins> QueryPinsByPos(const n0::SceneNodePtr& node,
		const sm::vec2& pos, sm::vec2& p_center);
	void QueryConnsByRect(const sm::rect& rect, std::vector<std::shared_ptr<Connecting>>& conns);

	bool QueryOrCreateNode(int x, int y, bool change_to);
    bool CreateNodeWithMousePos();
	bool CreateNode(int x, int y);

    void ClearSelectedConns();

    void CopyConnections();
    void PasteConnections();

    void SelectAllTree(const NodePtr& root) const;

private:
	ee0::WxStagePage& m_stage;

	const std::vector<NodePtr>& m_nodes;

	std::shared_ptr<Pins> m_selected_pin = nullptr;
#ifdef BP_CONNECT_PINS_OP_SELECT_CONNS
	std::vector<std::shared_ptr<Connecting>> m_selected_conns;
#endif // BP_CONNECT_PINS_OP_SELECT_CONNS

	sm::vec2 m_first_pos;
	sm::vec2 m_last_pos;

	gs::Bezier m_curve;

    // for paste connections
    std::vector<n0::SceneNodePtr> m_clipboard;

}; // ConnectPinsOP

}