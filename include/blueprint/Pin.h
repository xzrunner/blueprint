#pragma once

#include <painting0/Color.h>

#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace bp
{

class Node;
class Connecting;

enum PinType
{
	PIN_PORT = 0,

    PIN_ANY_VAR,

	// variable
	PIN_BOOLEAN,
	PIN_INTEGER,
	PIN_FLOAT,
	PIN_STRING,
	PIN_TEXT,
	PIN_VECTOR,
	PIN_ROTATOR,
	PIN_TRANSFORM,
	PIN_OBJECT,
};

class Pin
{
public:
	Pin(bool is_input, int pos, int type, const std::string& name, const Node& parent);
	~Pin();

	// for draw
    std::string GetDesc() const;
	const pt0::Color& GetColor() const;

	bool CanTypeCast(const Pin& p) const;

	bool IsInput() const { return m_is_input; }
	int  GetPosIdx() const { return m_pos; }
    void SetPosIdx(int idx) { m_pos = idx; }

	int  GetOldType() const { return m_old_type; }
    void SetOldType(int type) { m_old_type = type; }
	int  GetType() const { return m_new_type; }
	void SetType(int type);

	auto& GetParent() const { return m_parent; }

	void AddConnecting(const std::shared_ptr<Connecting>& conn);
	void RemoveConnecting(const std::shared_ptr<Connecting>& conn);
	void ClearConnecting();

	auto& GetConnecting() const { return m_connecting; }

	auto& GetName() const { return m_name; }
    void  SetName(const std::string& name) { m_name = name; }

public:
    struct ExtendFuncs
    {
        std::function<std::string(const std::string& name, int type)>
            get_desc_func = nullptr;
        std::function<const pt0::Color&(int type)>
            get_color_func = nullptr;
        std::function<bool(int type_from, int type_to)>
            can_type_cast_func = nullptr;
    };

    static void SetExtendFuncs(ExtendFuncs funcs) { m_funcs = funcs; }

private:
	// for parent
	bool m_is_input;
	int  m_pos;

	int m_old_type;
	int m_new_type;

	std::string m_name;

	const Node& m_parent;

	std::vector<std::shared_ptr<Connecting>> m_connecting;

    static ExtendFuncs m_funcs;

}; // Pin

}