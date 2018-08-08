#include "blueprint/CompNode.h"

#include <ee0/MsgHelper.h>

#include <node0/CompIdentity.h>
#include <node0/SceneNode.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <moon/runtime.h>
#include <moon/Blackboard.h>
#include <moon/Context.h>
#include <moon/Blackboard.h>
#include <moon/SceneNode.h>
#include <moon/runtime.h>

namespace
{

class Blueprint : public moon::Module
{
public:
	virtual moon::ModuleTypeID TypeID() const override {
		return moon::GetModuleTypeID<Blueprint>();
	}
	virtual const char* GetName() const override {
		return "moon.bp";
	}

}; // Blueprint

#define INSTANCE() (moon::Blackboard::Instance()->GetContext()->GetModuleMgr().GetModule<Blueprint>())

int w_new_node(lua_State* L)
{
	auto bb = moon::Blackboard::Instance();

//	const char* type = luaL_checkstring(L, 1);

	auto node = std::make_shared<n0::SceneNode>();
	// todo
	auto& cnode = node->AddSharedComp<bp::CompNode>();
	node->AddUniqueComp<n2::CompTransform>();
	sm::vec2 size(100, 100);
	cnode.GetNode().SetStyle(size, pt2::Color(255, 255, 0));
	node->AddUniqueComp<n0::CompIdentity>();
	node->AddUniqueComp<n2::CompBoundingBox>(sm::rect(size.x, size.y));

	moon::SceneNode* moon_node = nullptr;
	moon::luax_catchexcept(L, [&]() {
		moon_node = new moon::SceneNode(node);
	});
	moon::luax_pushtype(L, moon::SCENE_NODE_ID, moon_node);
	moon_node->Release();

#ifdef EASYEDITOR
	ee0::MsgHelper::InsertNode(*bb->GetSubMgr(), node);
#endif // EASYEDITOR

	return 0;
}

}

namespace moon
{

// List of functions to wrap.
static const luaL_Reg functions[] =
{
	{ "new_node", w_new_node },

	{ 0, 0 }
};

// Types for this module.
static const lua_CFunction types[] =
{
	0
};

extern "C" int luaopen_moon_bp(lua_State* L)
{
	Blueprint* instance = INSTANCE();
	if (instance == nullptr) {
		luax_catchexcept(L, [&](){ instance = new Blueprint(); });
	} else {
		instance->Retain();
	}

	WrappedModule w;
	w.module = instance;
	w.name = "bp";
	w.type = MODULE_ID;
	w.functions = functions;
	w.types = types;

	return luax_register_module(L, w);
}

}