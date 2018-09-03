#include "blueprint/NSCompNode.h"
#include "blueprint/CompNode.h"
#include "blueprint/NodeFactory.h"

namespace bp
{

size_t NSCompNode::GetBinSize(const std::string& dir) const
{
	return 0;
}

void NSCompNode::StoreToBin(const std::string& dir, bs::ExportStream& es) const
{
}

void NSCompNode::LoadFromBin(const std::string& dir, bs::ImportStream& is)
{
}

void NSCompNode::StoreToJson(const std::string& dir, rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const
{
	val.SetObject();

	rapidjson::Value node_val;
	node_val.SetObject();

	node_val.AddMember("type", rapidjson::Value(m_node->TypeName().c_str(), alloc), alloc);

	val.AddMember("node", node_val, alloc);
}

void NSCompNode::LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val)
{
	std::string type = val["node"]["type"].GetString();
	m_node = NodeFactory::Instance()->Create(type);
	
}

void NSCompNode::StoreToMem(CompNode& comp) const
{
	comp.SetNode(m_node);
}

void NSCompNode::LoadFromMem(const CompNode& comp)
{
	m_node = comp.GetNode();
}

}