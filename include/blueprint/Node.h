#pragma once

#include "blueprint/typedef.h"

#include <cu/cu_macro.h>
#include <SM_Vector.h>
#include <SM_Matrix2D.h>
#include <painting2/Color.h>
#include <cpputil/ClassInfo.h>

#include <rapidjson/document.h>

#include <vector>
#include <memory>

namespace mm { class LinearAllocator; }
namespace pt2 { class WindowContext; }
namespace pt3 { class WindowContext; }

namespace bp
{

using NodeTypeID = size_t;

namespace Internal
{
inline size_t GetUniqueNodeTypeID() noexcept
{
    static NodeTypeID id{ 0u };
    return id++;
}
}

template <typename T>
inline NodeTypeID GetNodeTypeID() noexcept
{
    static_assert(std::is_base_of<Node, T>::value,
        "T must inherit from Node");

    static NodeTypeID type_id{Internal::GetUniqueNodeTypeID()};
    return type_id;
}

class Pins;

struct UpdateParams
{
	UpdateParams(const std::shared_ptr<pt2::WindowContext>& wc2,
		const std::shared_ptr<pt3::WindowContext>& wc3)
		: wc2(wc2), wc3(wc3) {}

	std::shared_ptr<pt2::WindowContext> wc2 = nullptr;
	std::shared_ptr<pt3::WindowContext> wc3 = nullptr;
};

class Node
{
public:
	Node(const std::string& title);
	virtual ~Node() {}

	virtual NodeTypeID TypeID() const = 0;

	virtual void Draw(const sm::Matrix2D& mt) const;
	virtual bool Update(const UpdateParams& params) { return false; }

	virtual void StoreToJson(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const;
	virtual void LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir,
		const rapidjson::Value& val);

	virtual const cpputil::ClassInfo<bp::Node>& GetClassInfo() const = 0;

	auto& GetTitle() const { return m_title; }

	auto& GetAllInput() const { return m_all_input; }
	auto& GetAllOutput() const { return m_all_output; }

	auto& GetPos() const { return m_pos; }
	bool  SetPos(const sm::vec2& pos);

	void  SetName(const std::string& name) { m_name = name; }
	auto& GetName() const { return m_name; }

	static void Register(cpputil::ClassInfo<Node>* ci);

public:
	struct Style
	{
		float width;
		float height;

		int line_num;
	};

	const Style& GetStyle() const { return m_style; }
	void SetStyle(const Style& style) { m_style = style; }

protected:
	void AddPins(const std::shared_ptr<Pins>& pins);

	void SetWidth(float width);

	void Layout();

private:
	static bool CheckPinsName(const Pins& src,
		const std::vector<std::shared_ptr<Pins>>& dst);

protected:
	std::string m_title;

	Style m_style;

public:
	static const uint32_t STYLE_SMALL_TITLE_FONT = 0x00000001;
	static const uint32_t STYLE_ONLY_TITLE       = 0x00000002;

	static const uint32_t LIFE_DELETE_LATER      = 0x00000010;
	static const uint32_t SIZE_CHANGING          = 0x00000020;

	CU_FLAG_METHOD(StyleSmallTitleFont, STYLE_SMALL_TITLE_FONT);
	CU_FLAG_METHOD(StyleOnlyTitle,      STYLE_ONLY_TITLE);
	CU_FLAG_METHOD(LifeDeleteLater,     LIFE_DELETE_LATER);
	CU_FLAG_METHOD(SizeChanging,        SIZE_CHANGING);

private:
	std::string m_name;

	std::vector<std::shared_ptr<Pins>> m_all_input;
	std::vector<std::shared_ptr<Pins>> m_all_output;

	// for draw
	sm::vec2 m_pos;

	mutable uint32_t m_flags = 0;

}; // Node

}

#define DECLARE_NODE_CLASS(type)                                               \
public:                                                                        \
	virtual bp::NodeTypeID TypeID() const override {                           \
		return bp::GetNodeTypeID<type>();                                      \
	}                                                                          \
	virtual const cpputil::ClassInfo<bp::Node>& GetClassInfo() const {         \
		return m_s_classinfo;                                                  \
	}                                                                          \
                                                                               \
	static auto& GetClassName() { return m_s_classinfo.GetClassName(); }       \
                                                                               \
private:                                                                       \
	static cpputil::ClassInfo<bp::Node> m_s_classinfo;



#define IMPLEMENT_NODE_CLASS(type, name)                                       \
cpputil::ClassInfo<bp::Node> type::m_s_classinfo(                              \
	(#name),                                                                   \
	[]()->bp::NodePtr { return std::make_shared<type>(); },                    \
	[](cpputil::ClassInfo<bp::Node>* ci) { bp::Node::Register(ci); }           \
);