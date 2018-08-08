#pragma once

#include "blueprint/Node.h"

#include <node0/CompAsset.h>

namespace bp
{

class CompNode : public n0::CompAsset
{
public:
	virtual const char* Type() const override { return TYPE_NAME; }

	virtual n0::AssetID AssetTypeID() const override {
		return n0::GetAssetUniqueTypeID<CompNode>();
	}
	virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
		bool inverse = false) const override {}

	Node& GetNode() { return m_node; }
	const Node& GetNode() const { return m_node; }

	static const char* const TYPE_NAME;

private:
	Node m_node;

}; // CompNode

}