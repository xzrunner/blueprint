#pragma once

#include "blueprint/typedef.h"

#include <ns/NodeComponent.h>
#include <node0/typedef.h>

#include <vector>

namespace bp
{

class Pins;
class CompNode;

class NSCompNode : public ns::NodeComponent
{
public:
	NSCompNode() {}

	//
	// bin
	//
	virtual size_t GetBinSize(const std::string& dir) const override;
	virtual void   StoreToBin(const std::string& dir, bs::ExportStream& es) const override;
	virtual void   LoadFromBin(const std::string& dir, bs::ImportStream& is) override;

	//
	// json
	//
	virtual void StoreToJson(const std::string& dir, rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const override;
	virtual void LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val) override;

	//
	// memory
	//
	void StoreToMem(CompNode& comp) const;
	void LoadFromMem(const CompNode& comp);

	static void StoreConnection(const std::vector<n0::SceneNodePtr>& nodes,
		rapidjson::Value& nodes_val, rapidjson::MemoryPoolAllocator<>& alloc);
	static void LoadConnection(std::vector<n0::SceneNodePtr>& nodes,
		const rapidjson::Value& nodes_val);

private:
	static std::shared_ptr<Pins> QueryPinsByName(const Node& node, bool is_input, const std::string& name);

private:
	NodePtr m_node = nullptr;

}; // NSCompNode

}