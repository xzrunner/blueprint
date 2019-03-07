#pragma once

#include "blueprint/typedef.h"
#include "blueprint/NodeStyle.h"

#include <cu/cu_macro.h>
#include <SM_Vector.h>
#include <SM_Matrix2D.h>
#include <painting0/Color.h>

#include <rapidjson/document.h>
#include <rttr/registration>

#include <vector>
#include <memory>

namespace mm { class LinearAllocator; }
namespace pt2 { class WindowContext; }
namespace pt3 { class WindowContext; }

namespace bp
{

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

	virtual void Draw(const sm::Matrix2D& mt) const;
	virtual bool Update(const UpdateParams& params) { return false; }

	virtual void Refresh() {}

	auto& GetTitle() const { return m_title; }

	auto& GetAllInput() const { return m_all_input; }
	auto& GetAllOutput() const { return m_all_output; }

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

public:
	struct Style
	{
		float width = 0.0f;
		float height = 0.0f;

		int line_num = 0;

        pt0::Color panel_bg_col = BG_COLOR_DEFAULT;
	};

	const Style& GetStyle() const { return m_style; }
	void SetStyle(const Style& style) { m_style = style; }

protected:
	void AddPins(const std::shared_ptr<Pins>& pins);

protected:
	static bool CheckPinsName(const Pins& src,
		const std::vector<std::shared_ptr<Pins>>& dst);

	void Layout();

protected:
	std::string m_title;

	Style m_style;

	std::vector<std::shared_ptr<Pins>> m_all_input;
	std::vector<std::shared_ptr<Pins>> m_all_output;

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
	// for draw
	sm::vec2 m_pos;

	mutable uint32_t m_flags = 0;

	RTTR_ENABLE()

}; // Node

}