#include "blueprint/Evaluator.h"
#include "blueprint/Connecting.h"
#include "blueprint/node/Vector1.h"
#include "blueprint/node/Vector2.h"
#include "blueprint/node/Vector3.h"
#include "blueprint/node/Vector4.h"
#include "blueprint/node/Negate.h"
#include "blueprint/node/Add.h"
#include "blueprint/node/Subtract.h"
#include "blueprint/node/Multiply.h"
#include "blueprint/node/Divide.h"
#include "blueprint/node/Combine.h"
#include "blueprint/node/Split.h"
#include "blueprint/node/For.h"
#include "blueprint/node/Script.h"

#include <chaiscript/chaiscript.hpp>

namespace bp
{

float Evaluator::CalcFloat(const Connecting& conn, int for_idx)
{
    float ret = 0;

    auto& node = conn.GetFrom()->GetParent();
    auto node_type = node.get_type();
    if (node_type == rttr::type::get<node::Vector1>())
    {
        ret = static_cast<const node::Vector1&>(node).GetValue();
    }
    else if (node_type == rttr::type::get<node::Negate>())
    {
        auto& conns = node.GetAllInput()[0]->GetConnecting();
        ret = conns.empty() ? 0 : -CalcFloat(*conns[0], for_idx);
    }
    else if (node_type == rttr::type::get<node::Add>())
    {
        auto& conns_a = node.GetAllInput()[node::Add::ID_A]->GetConnecting();
        float a = conns_a.empty() ? 0 : CalcFloat(*conns_a[0], for_idx);

        auto& conns_b = node.GetAllInput()[node::Add::ID_B]->GetConnecting();
        float b = conns_b.empty() ? 0 : CalcFloat(*conns_b[0], for_idx);

        ret = a + b;
    }
    else if (node_type == rttr::type::get<node::Subtract>())
    {
        auto& conns_a = node.GetAllInput()[node::Subtract::ID_A]->GetConnecting();
        float a = conns_a.empty() ? 0 : CalcFloat(*conns_a[0], for_idx);

        auto& conns_b = node.GetAllInput()[node::Subtract::ID_B]->GetConnecting();
        float b = conns_b.empty() ? 0 : CalcFloat(*conns_b[0], for_idx);

        ret = a - b;
    }
    else if (node_type == rttr::type::get<node::Multiply>())
    {
        auto& conns_a = node.GetAllInput()[node::Multiply::ID_A]->GetConnecting();
        float a = conns_a.empty() ? 0 : CalcFloat(*conns_a[0], for_idx);

        auto& conns_b = node.GetAllInput()[node::Multiply::ID_B]->GetConnecting();
        float b = conns_b.empty() ? 0 : CalcFloat(*conns_b[0], for_idx);

        ret = a * b;
    }
    else if (node_type == rttr::type::get<node::Divide>())
    {
        auto& conns_a = node.GetAllInput()[node::Divide::ID_A]->GetConnecting();
        float a = conns_a.empty() ? 0 : CalcFloat(*conns_a[0], for_idx);

        auto& conns_b = node.GetAllInput()[node::Divide::ID_B]->GetConnecting();
        float b = conns_b.empty() ? 0 : CalcFloat(*conns_b[0], for_idx);

        ret = a / b;
    }
    else if (node_type == rttr::type::get<node::For>())
    {
        ret = static_cast<float>(for_idx);
    }
    else if (node_type == rttr::type::get<node::Script>())
    {
        chaiscript::ChaiScript chai;
        chai.add(chaiscript::var(for_idx), "i");
        try {
            ret = chai.eval<float>(static_cast<const node::Script&>(node).GetText());
        } catch (const chaiscript::exception::eval_error &e) {
            std::cout << "ChaiScript Error\n" << e.pretty_print() << '\n';
        }
    }
    else
    {
        auto type = node_type.get_name().to_string();
        printf("unknown type %s\n", type.c_str());
        assert(0);
    }

    return ret;
}

sm::vec3 Evaluator::CalcFloat3(const Connecting& conn, int for_idx)
{
    sm::vec3 ret;

    auto& node = conn.GetFrom()->GetParent();
    auto node_type = node.get_type();
    if (node_type == rttr::type::get<node::Vector3>())
    {
        ret = static_cast<const node::Vector3&>(node).GetValue();
    }
    else if (node_type == rttr::type::get<node::Negate>())
    {
        auto& conns = node.GetAllInput()[0]->GetConnecting();
        ret = conns.empty() ? sm::vec3(0, 0, 0) : CalcFloat3(*conns[0], for_idx);
    }
    else if (node_type == rttr::type::get<node::Combine>())
    {
        auto& conns_r = node.GetAllInput()[node::Combine::ID_R]->GetConnecting();
        float r = conns_r.empty() ? 0 : CalcFloat(*conns_r[0], for_idx);

        auto& conns_g = node.GetAllInput()[node::Combine::ID_G]->GetConnecting();
        float g = conns_g.empty() ? 0 : CalcFloat(*conns_g[0], for_idx);

        auto& conns_b = node.GetAllInput()[node::Combine::ID_B]->GetConnecting();
        float b = conns_b.empty() ? 0 : CalcFloat(*conns_b[0], for_idx);

        ret = sm::vec3(r, g, b);
    }
    else if (node_type == rttr::type::get<node::Add>())
    {
        auto& conns_a = node.GetAllInput()[node::Add::ID_A]->GetConnecting();
        sm::vec3 a = conns_a.empty() ? sm::vec3(0, 0, 0) : CalcFloat3(*conns_a[0], for_idx);

        auto& conns_b = node.GetAllInput()[node::Add::ID_B]->GetConnecting();
        sm::vec3 b = conns_b.empty() ? sm::vec3(0, 0, 0) : CalcFloat3(*conns_b[0], for_idx);

        ret = a + b;
    }
    else if (node_type == rttr::type::get<node::Subtract>())
    {
        auto& conns_a = node.GetAllInput()[node::Subtract::ID_A]->GetConnecting();
        sm::vec3 a = conns_a.empty() ? sm::vec3(0, 0, 0) : CalcFloat3(*conns_a[0], for_idx);

        auto& conns_b = node.GetAllInput()[node::Subtract::ID_B]->GetConnecting();
        sm::vec3 b = conns_b.empty() ? sm::vec3(0, 0, 0) : CalcFloat3(*conns_b[0], for_idx);

        ret = a - b;
    }
    else if (node_type == rttr::type::get<node::Script>())
    {
        chaiscript::ChaiScript chai;
        chai.add(chaiscript::var(for_idx), "i");
        chai.add(chaiscript::user_type<sm::vec3>(), "vec3");
        chai.add(chaiscript::constructor<sm::vec3(float x, float y, float z)>(), "vec3");
        try {
            ret = chai.eval<sm::vec3>(static_cast<const node::Script&>(node).GetText());
        } catch (const chaiscript::exception::eval_error &e) {
            std::cout << "ChaiScript Error\n" << e.pretty_print() << '\n';
        }
    }
    else
    {
        auto type = node_type.get_name().to_string();
        printf("unknown type %s\n", type.c_str());
        assert(0);
    }

    return ret;
}

}