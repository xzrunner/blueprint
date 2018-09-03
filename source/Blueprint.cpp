#include "blueprint/Blueprint.h"
#include "blueprint/CompNode.h"
#include "blueprint/NSCompNode.h"
#include "blueprint/RenderSystem.h"
#include "blueprint/NodeFactory.h"

#include <ns/CompIdxMgr.h>
#include <ns/RegistCallback.h>
#include <node2/RenderSystem.h>
#include <node2/UpdateSystem.h>
#include <node2/CompTransform.h>

namespace bp
{

void Blueprint::Init()
{
	// node serializer
	ns::CompIdxMgr::Instance()->AddExtTypeToIdx(CompNode::TYPE_NAME, ns::COMP_INVALID);
	ns::RegistCallback::AddUniqueCB<CompNode, NSCompNode>();

	// node2
	n2::RenderSystem::Instance()->AddDrawCompFunc(
		[](const n0::SceneNode& node, const n2::RenderParams& rp)
	{
		if (node.HasUniqueComp<CompNode>()) {
			auto& cnode = node.GetUniqueComp<CompNode>();
			RenderSystem::Instance()->DrawNode(*cnode.GetNode(), rp.GetMatrix());
		}
	});
	n2::UpdateSystem::Instance()->AddUpdateCompFunc([](const n0::SceneNode& node)->bool
	{
		if (!node.HasUniqueComp<CompNode>()) {
			return false;
		}

		auto& cnode = node.GetUniqueComp<CompNode>();
		auto& ctrans = node.GetUniqueComp<n2::CompTransform>();
		auto& pos = ctrans.GetTrans().GetPosition();
		return cnode.GetNode()->SetPos(pos);
	});
}

}