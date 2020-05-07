#pragma once

#include "blueprint/typedef.h"
#include "blueprint/NodeStyle.h"
#include "blueprint/Variant.h"

#include <cu/cu_macro.h>
#include <SM_Vector.h>
#include <SM_Matrix2D.h>
#include <painting0/Color.h>

#include <rttr/registration>
#include <rapidjson/document.h>

#include <vector>
#include <memory>

namespace mm { class LinearAllocator; }
namespace ur { class Device; class Context; }
namespace pt2 { class WindowContext; }
namespace pt3 { class WindowContext; }
namespace n2 { class RenderParams; }

namespace bp
{

class Pin;

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

	virtual void Draw(const ur::Device& dev, ur::Context& ctx,
        const n2::RenderParams& rp) const;
	virtual bool Update(const UpdateParams& params) { return false; }

    virtual void StoreToJson(const std::string& dir, rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) {}
    virtual void LoadFromJson(const std::string& dir, const rapidjson::Value& val) {}

	virtual void Refresh() {}

	auto& GetTitle() const { return m_title; }

	auto& GetAllInput() const { return m_all_input; }
	auto& GetAllOutput() const { return m_all_output; }

    auto& GetProps() const { return m_props; }

    bool UpdateExtInputPorts(bool is_connecting);
    void PrepareExtInputPorts(int count);

	auto& GetPos() const { return m_pos; }
	bool  SetPos(const sm::vec2& pos);

	// wrapper for serialize
	struct Flags
	{
		bool only_title;
		bool small_title_font;
	};
	Flags GetFlags() const;
	void SetFlags(Flags flags);

    auto& GetName() const { return m_name; }
    void  SetName(const std::string& name) { m_name = name; }

public:
	struct Style
	{
		float width = 0.0f;
		float height = 0.0f;

		int line_num = 0;

        pt0::Color panel_bg_col = BG_COLOR_DEFAULT;

        bool hori = true;

        bool draw_pin_label = true;
	};

	const Style& GetStyle() const { return m_style; }
	void SetStyle(const Style& style) { m_style = style; }

    static bool CheckPinName(const Pin& src,
        const std::vector<std::shared_ptr<Pin>>& dst);

protected:
	void AddPin(const std::shared_ptr<Pin>& pin);

	void Layout();

private:
    virtual std::string GenInputPinName(size_t idx) const;

protected:
	std::string m_title;

	Style m_style;

	std::vector<std::shared_ptr<Pin>> m_all_input;
	std::vector<std::shared_ptr<Pin>> m_all_output;

    std::vector<Variant> m_props;

public:
	static const uint32_t STYLE_SMALL_TITLE_FONT = 0x00000001;
	static const uint32_t STYLE_ONLY_TITLE       = 0x00000002;

	static const uint32_t LIFE_DELETE_LATER      = 0x00000010;
	static const uint32_t SIZE_CHANGED           = 0x00000020;
	static const uint32_t GET_VAR_NAME_CHANGED   = 0x00000040;

    static const uint32_t EXTENSIBLE_INPUT_PORTS = 0x00000100;

    static const uint32_t EDIT_NOT_DIRTY         = 0x00001000;

	CU_FLAG_METHOD(StyleSmallTitleFont,  STYLE_SMALL_TITLE_FONT);
	CU_FLAG_METHOD(StyleOnlyTitle,       STYLE_ONLY_TITLE);
	CU_FLAG_METHOD(LifeDeleteLater,      LIFE_DELETE_LATER);
	CU_FLAG_METHOD(SizeChanged,          SIZE_CHANGED);
    CU_FLAG_METHOD(GetVarNameChanged,    GET_VAR_NAME_CHANGED);
    CU_FLAG_METHOD(ExtensibleInputPorts, EXTENSIBLE_INPUT_PORTS);
    CU_FLAG_METHOD(EditNotDirty,         EDIT_NOT_DIRTY);

private:
	// for draw
	sm::vec2 m_pos;

	mutable uint32_t m_flags = 0;

    std::string m_name;

	RTTR_ENABLE()

}; // Node

}