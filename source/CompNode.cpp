#include "blueprint/CompNode.h"
#include "blueprint/Node.h"

#include <js/RTTR.h>

namespace bp
{

const char* const CompNode::TYPE_NAME = "bp_node";

std::unique_ptr<n0::NodeComp> CompNode::Clone(const n0::SceneNode& node) const
{
	auto comp = std::make_unique<CompNode>();
    if (!m_node) {
        return comp;
    }

    auto type = m_node->get_type();
    auto ctor = type.get_constructor();
    auto var = ctor.invoke();
    auto dst = var.get_value<std::shared_ptr<Node>>();
    comp->SetNode(dst);

    for (auto& prop : type.get_properties())
    {
        if (prop.get_metadata(js::RTTR::NoSerializeTag())) {
            continue;
        }

        rttr::variant prop_value = prop.get_value(m_node);
        if (!prop_value) {
            continue;
        }

        prop.set_value(dst, prop_value);
    }

	return comp;
}

}