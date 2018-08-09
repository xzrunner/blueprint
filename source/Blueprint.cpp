#include "blueprint/Blueprint.h"
#include "blueprint/CompNode.h"
#include "blueprint/RenderSystem.h"

#include <node2/RenderSystem.h>

namespace bp
{

void Blueprint::Init()
{
	n2::RenderSystem::Instance()->AddDrawAssetFunc(
		n0::GetAssetUniqueTypeID<CompNode>(),
		[](const n0::CompAsset& casset, const n2::RenderParams& rp)
	{
		auto& cnode = static_cast<const CompNode&>(casset);
		RenderSystem::Instance()->DrawNode(*cnode.GetNode(), rp.GetMatrix());
	});
}

}