#pragma once

#include <ns/NodeComponent.h>

namespace bp
{

class Node;

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

private:
	std::shared_ptr<Node> m_node = nullptr;

}; // NSCompNode

}