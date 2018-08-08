//#include "blueprint/CompGroup.h"
//
//namespace bp
//{
//
//const char* const CompGroup::TYPE_NAME = "bp_graph";
//
//std::unique_ptr<n0::NodeComp> CompGroup::Clone(const n0::SceneNode& node) const
//{
//	auto comp = std::make_unique<CompGroup>();
//	comp->m_group = m_group;
//	return comp;
//}
//
//}