#pragma once

#include "blueprint/Node.h"

#include <node0/NodeComp.h>

namespace bp
{

class CompNode : public n0::NodeComp
{
public:
	CompNode() {}
	virtual const char* Type() const override { return TYPE_NAME; }
	virtual n0::CompID TypeID() const override {
		return n0::GetCompTypeID<CompNode>();
	}
	virtual std::unique_ptr<n0::NodeComp> Clone(const n0::SceneNode& node) const override;

	auto& GetNode() { return m_node; }
	auto& GetNode() const { return m_node; }

	void SetNode(const std::shared_ptr<Node>& node) {
		m_node = node;
	}

	static const char* const TYPE_NAME;

private:
	std::shared_ptr<Node> m_node = nullptr;

}; // CompNode

}