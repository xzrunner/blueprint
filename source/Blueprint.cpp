#include "blueprint/Blueprint.h"
#include "blueprint/CompNode.h"
#include "blueprint/NSCompNode.h"
#include "blueprint/Node.h"
#include "blueprint/SerializeHelper.h"
#include "blueprint/node/Commentary.h"
#include "blueprint/node/SetReference.h"
#include "blueprint/node/GetReference.h"
#include "blueprint/node/SetValue.h"
#include "blueprint/node/GetValue.h"
#include "blueprint/node/Proxy.h"
#include "blueprint/node/Hub.h"
#include "blueprint/node/For.h"
#include "blueprint/node/For2.h"
#include "blueprint/node/Script.h"
#include "blueprint/node/Switch.h"
#include "blueprint/node/CompareLess.h"
#include "blueprint/node/CompareGreater.h"
#include "blueprint/node/CompareEqual.h"
#include "blueprint/node/CompareNotEqual.h"
#include "blueprint/node/Boolean.h"
#include "blueprint/node/Vector1.h"
#include "blueprint/node/Vector2.h"
#include "blueprint/node/Vector3.h"
#include "blueprint/node/Vector4.h"
#include "blueprint/node/Function.h"
#include "blueprint/node/Input.h"
#include "blueprint/node/Output.h"
#include "blueprint/node/Abs.h"

#include <ns/CompIdxMgr.h>
#include <ns/RegistCallback.h>
#include <node2/RenderSystem.h>
#include <node2/UpdateSystem.h>
#include <node2/CompTransform.h>
#include <node2/AABBSystem.h>

namespace bp
{

CU_SINGLETON_DEFINITION(Blueprint);

extern void regist_rttr();

Blueprint::Blueprint()
{
	Init();
	InitNodes();
}

void Blueprint::Init()
{
	regist_rttr();

	// node serializer
	ns::CompIdxMgr::Instance()->AddExtTypeToIdx(CompNode::TYPE_NAME, ns::COMP_INVALID);
	ns::RegistCallback::AddUniqueCB<CompNode, NSCompNode>();

	// node2
	n2::RenderSystem::Instance()->AddDrawCompFunc(
		[](const ur::Device& dev, ur::Context& ctx, const n0::SceneNode& node, const n2::RenderParams& rp)
	{
		if (node.HasUniqueComp<CompNode>())
        {
			auto& cnode = node.GetUniqueComp<CompNode>();
			cnode.GetNode()->Draw(dev, ctx, rp);
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
	n2::AABBSystem::Instance()->AddGetBoundFunc([](const n0::SceneNode& node, sm::rect& bound)->bool
	{
		if (!node.HasUniqueComp<CompNode>()) {
			return false;
		}

		auto& cnode = node.GetUniqueComp<CompNode>();
		auto& st = cnode.GetNode()->GetStyle();
		bound.Build(st.width, st.height);
		return true;
	});

	// setup conns
	SerializeHelper::SetupConnCB();
}

void Blueprint::InitNodes()
{
    //auto list = rttr::type::get<Node>().get_derived_classes();
    //m_nodes.reserve(list.size());
    //for (auto& t : list)
    //{
    //    auto obj = t.create();
    //    assert(obj.is_valid());
    //    auto node = obj.get_value<bp::NodePtr>();
    //    assert(node);
    //    m_nodes.push_back(node);
    //}

    m_nodes.push_back(std::make_shared<node::Commentary>());
    m_nodes.push_back(std::make_shared<node::SetReference>());
    m_nodes.push_back(std::make_shared<node::GetReference>());
    m_nodes.push_back(std::make_shared<node::SetValue>());
    m_nodes.push_back(std::make_shared<node::GetValue>());
    m_nodes.push_back(std::make_shared<node::Proxy>());
    m_nodes.push_back(std::make_shared<node::Hub>());
    m_nodes.push_back(std::make_shared<node::For>());
    m_nodes.push_back(std::make_shared<node::For2>());
    m_nodes.push_back(std::make_shared<node::Script>());
    m_nodes.push_back(std::make_shared<node::Switch>());
    m_nodes.push_back(std::make_shared<node::CompareLess>());
    m_nodes.push_back(std::make_shared<node::CompareGreater>());
    m_nodes.push_back(std::make_shared<node::CompareEqual>());
    m_nodes.push_back(std::make_shared<node::CompareNotEqual>());
    m_nodes.push_back(std::make_shared<node::Boolean>());
    m_nodes.push_back(std::make_shared<node::Vector1>());
    m_nodes.push_back(std::make_shared<node::Vector2>());
    m_nodes.push_back(std::make_shared<node::Vector3>());
    m_nodes.push_back(std::make_shared<node::Vector4>());
    m_nodes.push_back(std::make_shared<node::Function>());
    m_nodes.push_back(std::make_shared<node::Input>());
    m_nodes.push_back(std::make_shared<node::Output>());
    m_nodes.push_back(std::make_shared<node::Abs>());
}

}