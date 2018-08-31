#pragma once

#include <SM_Vector.h>
#include <painting2/Color.h>
#include <node0/typedef.h>

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
    static_assert(std::is_base_of<node::Node, T>::value,
        "T must inherit from Node");

    static NodeTypeID type_id{Internal::GetUniqueNodeTypeID()};
    return type_id;
}

namespace node
{

class Pins;

class Node
{
public:
	Node(const std::string& title);
	virtual ~Node() {}

	virtual NodeTypeID TypeID() const = 0;
	virtual std::string TypeName() const = 0;
	virtual std::shared_ptr<Node> Create() const = 0;

	auto& GetTitle() const { return m_title; }

	auto& GetAllInput() const { return m_all_input; }
	auto& GetAllOutput() const { return m_all_output; }

	void SetParent(const n0::SceneNodePtr& parent) {
		m_parent = parent;
	}
	auto& GetParent() const { return m_parent; }

	auto& GetLastPos() const { return m_last_pos; }
	void  SetLastPos(const sm::vec2& pos) const { m_last_pos = pos; }

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
	std::string m_title;

	Style m_style;

	std::vector<std::shared_ptr<Pins>> m_all_input;
	std::vector<std::shared_ptr<Pins>> m_all_output;

	// for draw
	n0::SceneNodePtr m_parent = nullptr;
	mutable sm::vec2 m_last_pos;

}; // Node

}
}