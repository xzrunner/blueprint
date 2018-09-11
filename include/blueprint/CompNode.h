#pragma once

#include "blueprint/typedef.h"

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

	void SetNode(const NodePtr& node) {
		m_node = node;
	}

	static const char* const TYPE_NAME;

private:
	NodePtr m_node = nullptr;

}; // CompNode

}