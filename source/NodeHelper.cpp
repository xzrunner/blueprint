#include "blueprint/NodeHelper.h"
#include "blueprint/Connecting.h"
#include "blueprint/Pin.h"
#include "blueprint/Node.h"
#include "blueprint/NSCompNode.h"
#include "blueprint/CompNode.h"
#include "blueprint/node/Function.h"

#include <ns/CompFactory.h>
#include <js/RapidJsonHelper.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompBoundingBox.h>

#include <boost/filesystem.hpp>

namespace bp
{

void NodeHelper::RemoveDefaultNode(const bp::Pin& p)
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

void NodeHelper::LoadFunctionNode(const ur::Device& dev, const n0::SceneNodePtr& obj, const bp::NodePtr& node)
{
    assert(node->get_type() == rttr::type::get<bp::node::Function>());

    auto func_node = std::static_pointer_cast<bp::node::Function>(node);
    if (!func_node->IsNeedLoad()) {
        return;
    }
    auto& filepath = func_node->GetFilepath();
    if (filepath.empty()) {
        return;
    }
    n0::CompAssetPtr casset = ns::CompFactory::Instance()->CreateAsset(dev, filepath);
    if (!casset) {
        return;
    }
    assert(casset->TypeID() == n0::GetAssetUniqueTypeID<n0::CompComplex>());
    auto& ccomplex = std::static_pointer_cast<n0::CompComplex>(casset);
    auto& children = ccomplex->GetAllChildren();
    bp::node::Function::SetChildren(func_node, children);

    // load children function node
    for (auto& c : children)
    {
        auto& cnode = c->GetUniqueComp<CompNode>();
        auto bp_node = cnode.GetNode();
        if (bp_node->get_type() == rttr::type::get<node::Function>()) {
            NodeHelper::LoadFunctionNode(dev, c, bp_node);
        }
    }

    // load conns
    rapidjson::Document doc;
    js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);
    bp::NSCompNode::LoadConnection(children, doc["nodes"]);

    // update aabb
    auto& st = node->GetStyle();
    obj->GetUniqueComp<n2::CompBoundingBox>().SetSize(
        *obj, sm::rect(st.width, st.height)
    );
}

sm::Matrix2D NodeHelper::CalcPreviewMat(const Node& node, const sm::Matrix2D& mt)
{
	auto& style = node.GetStyle();
	const float LEN = style.width;
	sm::rect r;
	r.xmin = -style.width * 0.5f;  r.xmax = r.xmin + LEN;
	r.ymax = -style.height * 0.5f; r.ymin = r.ymax - LEN;

	sm::Matrix2D ret = mt;
	ret.x[0] *= r.Width();
	ret.x[3] *= r.Height();
	ret.x[4] += r.Center().x;
	ret.x[5] += r.Center().y;
	return ret;
}

}