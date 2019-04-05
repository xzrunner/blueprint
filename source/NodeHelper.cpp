#include "blueprint/NodeHelper.h"
#include "blueprint/Connecting.h"
#include "blueprint/Pins.h"
#include "blueprint/Node.h"
#include "blueprint/NSCompNode.h"
#include "blueprint/node/Function.h"

#include <ns/CompFactory.h>
#include <js/RapidJsonHelper.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompBoundingBox.h>

#include <boost/filesystem.hpp>

namespace bp
{

void NodeHelper::RemoveDefaultNode(const bp::Pins& p)
{
	auto& conns = p.GetConnecting();
	if (conns.empty()) {
		return;
	}
	auto& pair = conns[0]->GetFrom();
	assert(pair);
    if (pair->GetConnecting().size() == 1)
    {
        auto& parent = pair->GetParent();
        // fixme: is default input param
        if (parent.IsStyleOnlyTitle()) {
            parent.SetLifeDeleteLater(true);
        }
    }
}

void NodeHelper::LoadFunctionNode(const n0::SceneNodePtr& obj, const bp::NodePtr& node)
{
    assert(node->get_type() == rttr::type::get<bp::node::Function>());

    auto func_node = std::static_pointer_cast<bp::node::Function>(node);
    auto& filepath = func_node->GetFilepath();
    if (filepath.empty()) {
        return;
    }
    n0::CompAssetPtr casset = ns::CompFactory::Instance()->CreateAsset(filepath);
    if (!casset) {
        return;
    }
    assert(casset->TypeID() == n0::GetAssetUniqueTypeID<n0::CompComplex>());
    auto& ccomplex = std::static_pointer_cast<n0::CompComplex>(casset);
    auto& children = ccomplex->GetAllChildren();
    bp::node::Function::SetChildren(func_node, children);

    LoadConnections(children, filepath);

    // update aabb
    auto& st = node->GetStyle();
    obj->GetUniqueComp<n2::CompBoundingBox>().SetSize(
        *obj, sm::rect(st.width, st.height)
    );
}

void NodeHelper::LoadConnections(const std::vector<n0::SceneNodePtr>& nodes,
                                 const std::string& filepath)
{
	auto dir = boost::filesystem::path(filepath).parent_path().string();
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

	bp::NSCompNode::LoadConnection(nodes, doc["nodes"]);
}

}