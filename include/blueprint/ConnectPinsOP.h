#pragma once

#include <ee0/EditOP.h>

#include <SM_Vector.h>
#include <node0/typedef.h>
#include <painting2/BezierShape.h>

namespace ee0 { class WxStagePage; }

namespace bp
{
namespace node { class Pins; }

class ConnectPinsOP : public ee0::EditOP
{
public:
	ConnectPinsOP(const std::shared_ptr<pt0::Camera>& cam,
		ee0::WxStagePage& stage);

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseLeftUp(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;

	virtual bool OnDraw() const;

private:
	std::shared_ptr<node::Pins> QueryPinsByPos(const n0::SceneNodePtr& node,
		const sm::vec2& pos, sm::vec2& p_center);

	bool QueryOrCreateNode(int x, int y);
	bool CreateNode(int x, int y);

private:
	ee0::WxStagePage& m_stage;

	std::shared_ptr<node::Pins> m_selected = nullptr;

	sm::vec2 m_first_pos;
	sm::vec2 m_last_pos;

	pt2::BezierShape m_curve;

}; // ConnectPinsOP

}