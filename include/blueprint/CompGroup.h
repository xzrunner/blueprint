//#pragma once
//
//#include "blueprint/Group.h"
//
//#include <node0/NodeComp.h>
//
//namespace bp
//{
//
//class CompGroup : public n0::NodeComp
//{
//public:
//	virtual const char* Type() const override { return TYPE_NAME; }
//	virtual n0::CompID TypeID() const override {
//		return n0::GetCompTypeID<CompGroup>();
//	}
//	virtual std::unique_ptr<n0::NodeComp> Clone(const n0::SceneNode& node) const override;
//
//	const Group& GetGraph() const { return m_group; }
//
//	static const char* const TYPE_NAME;
//
//private:
//	Group m_group;
//
//}; // CompGroup
//
//}