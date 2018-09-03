#pragma once

#include <SM_Vector.h>
#include <painting2/Color.h>
#include <SM_Matrix2D.h>

#include <vector>
#include <memory>

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

class Node
{
public:
	Node(const std::string& title);
	virtual ~Node() {}

	virtual NodeTypeID TypeID() const = 0;
	virtual const std::string& TypeName() const = 0;
	virtual std::shared_ptr<Node> Create() const = 0;
	virtual void Draw(const sm::Matrix2D& mt) const {}

	auto& GetTitle() const { return m_title; }

	auto& GetAllInput() const { return m_all_input; }
	auto& GetAllOutput() const { return m_all_output; }

	auto& GetPos() const { return m_pos; }
	bool  SetPos(const sm::vec2& pos);

public:
	struct Style
	{
		float width;
		float height;

		int line_num;

		bool small_title = false;
	};

	const Style& GetStyle() const { return m_style; }
	void SetStyle(const Style& style) { m_style = style; }

protected:
	void AddPins(const std::shared_ptr<Pins>& pins);

	void SetWidth(float width);

	void Layout();

protected:
	std::string m_title;

	Style m_style;

private:
	std::vector<std::shared_ptr<Pins>> m_all_input;
	std::vector<std::shared_ptr<Pins>> m_all_output;

	// for draw
	sm::vec2 m_pos;

}; // Node

}