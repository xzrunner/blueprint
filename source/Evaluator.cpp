#include "blueprint/Evaluator.h"
#include "blueprint/Connecting.h"
#include "blueprint/ScriptEnv.h"

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
#include "blueprint/node/For2.h"
#include "blueprint/node/Script.h"

#include <cpputil/StringHelper.h>

#include <chaiscript/chaiscript.hpp>

#include <stack>

namespace
{

void prepare_script_vars(const bp::Node& node)
{
    auto& chai = bp::ScriptEnv::Instance()->GetChai();

    auto& conns_i = node.GetAllInput()[bp::node::Script::ID_VAR_I]->GetConnecting();
    if (!conns_i.empty())
    {
        int i = static_cast<int>(bp::Evaluator::CalcFloat(*conns_i[0]));
        chai->add(chaiscript::var(i), "i");
    }

    auto& conns_j = node.GetAllInput()[bp::node::Script::ID_VAR_J]->GetConnecting();
    if (!conns_j.empty())
    {
        int j = static_cast<int>(bp::Evaluator::CalcFloat(*conns_j[0]));
        chai->add(chaiscript::var(j), "j");
    }
}

}

namespace bp
{

float Evaluator::CalcFloat(const Connecting& conn)
{
    float ret = 0;

    auto& node = conn.GetFrom()->GetParent();
    auto node_type = node.get_type();
    auto pos_idx = conn.GetFrom()->GetPosIdx();
    if (node_type == rttr::type::get<node::Vector1>())
    {
        ret = static_cast<const node::Vector1&>(node).GetValue();
    }
    else if (node_type == rttr::type::get<node::Negate>())
    {
        auto& conns = node.GetAllInput()[0]->GetConnecting();
        ret = conns.empty() ? 0 : -CalcFloat(*conns[0]);
    }
    else if (node_type == rttr::type::get<node::Add>())
    {
        auto& conns_a = node.GetAllInput()[node::Add::ID_A]->GetConnecting();
        float a = conns_a.empty() ? 0 : CalcFloat(*conns_a[0]);

        auto& conns_b = node.GetAllInput()[node::Add::ID_B]->GetConnecting();
        float b = conns_b.empty() ? 0 : CalcFloat(*conns_b[0]);

        ret = a + b;
    }
    else if (node_type == rttr::type::get<node::Subtract>())
    {
        auto& conns_a = node.GetAllInput()[node::Subtract::ID_A]->GetConnecting();
        float a = conns_a.empty() ? 0 : CalcFloat(*conns_a[0]);

        auto& conns_b = node.GetAllInput()[node::Subtract::ID_B]->GetConnecting();
        float b = conns_b.empty() ? 0 : CalcFloat(*conns_b[0]);

        ret = a - b;
    }
    else if (node_type == rttr::type::get<node::Multiply>())
    {
        auto& conns_a = node.GetAllInput()[node::Multiply::ID_A]->GetConnecting();
        float a = conns_a.empty() ? 0 : CalcFloat(*conns_a[0]);

        auto& conns_b = node.GetAllInput()[node::Multiply::ID_B]->GetConnecting();
        float b = conns_b.empty() ? 0 : CalcFloat(*conns_b[0]);

        ret = a * b;
    }
    else if (node_type == rttr::type::get<node::Divide>())
    {
        auto& conns_a = node.GetAllInput()[node::Divide::ID_A]->GetConnecting();
        float a = conns_a.empty() ? 0 : CalcFloat(*conns_a[0]);

        auto& conns_b = node.GetAllInput()[node::Divide::ID_B]->GetConnecting();
        float b = conns_b.empty() ? 0 : CalcFloat(*conns_b[0]);

        ret = a / b;
    }
    else if (node_type == rttr::type::get<node::For>())
    {
        if (pos_idx == node::For::ID_INDEX) {
            ret = static_cast<float>(static_cast<const node::For&>(node).index_curr);
        }
    }
    else if (node_type == rttr::type::get<node::For2>())
    {
        switch (pos_idx)
        {
        case node::For2::ID_INDEX_0:
            ret = static_cast<float>(static_cast<const node::For2&>(node).i_curr);
            break;
        case node::For2::ID_INDEX_1:
            ret = static_cast<float>(static_cast<const node::For2&>(node).j_curr);
            break;
        }
    }
    else if (node_type == rttr::type::get<node::Script>())
    {
        auto text = static_cast<const node::Script&>(node).GetText();
        cpputil::StringHelper::ReplaceAll(text, "\\n", "\n");
        cpputil::StringHelper::ReplaceAll(text, "\\t", "\t");

        prepare_script_vars(node);
        try {
            ret = ScriptEnv::Instance()->GetChai()->eval<float>(text);
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

sm::vec3 Evaluator::CalcFloat3(const Connecting& conn)
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
        ret = conns.empty() ? sm::vec3(0, 0, 0) : CalcFloat3(*conns[0]);
    }
    else if (node_type == rttr::type::get<node::Combine>())
    {
        auto& conns_r = node.GetAllInput()[node::Combine::ID_R]->GetConnecting();
        float r = conns_r.empty() ? 0 : CalcFloat(*conns_r[0]);

        auto& conns_g = node.GetAllInput()[node::Combine::ID_G]->GetConnecting();
        float g = conns_g.empty() ? 0 : CalcFloat(*conns_g[0]);

        auto& conns_b = node.GetAllInput()[node::Combine::ID_B]->GetConnecting();
        float b = conns_b.empty() ? 0 : CalcFloat(*conns_b[0]);

        ret = sm::vec3(r, g, b);
    }
    else if (node_type == rttr::type::get<node::Add>())
    {
        auto& conns_a = node.GetAllInput()[node::Add::ID_A]->GetConnecting();
        sm::vec3 a = conns_a.empty() ? sm::vec3(0, 0, 0) : CalcFloat3(*conns_a[0]);

        auto& conns_b = node.GetAllInput()[node::Add::ID_B]->GetConnecting();
        sm::vec3 b = conns_b.empty() ? sm::vec3(0, 0, 0) : CalcFloat3(*conns_b[0]);

        ret = a + b;
    }
    else if (node_type == rttr::type::get<node::Subtract>())
    {
        auto& conns_a = node.GetAllInput()[node::Subtract::ID_A]->GetConnecting();
        sm::vec3 a = conns_a.empty() ? sm::vec3(0, 0, 0) : CalcFloat3(*conns_a[0]);

        auto& conns_b = node.GetAllInput()[node::Subtract::ID_B]->GetConnecting();
        sm::vec3 b = conns_b.empty() ? sm::vec3(0, 0, 0) : CalcFloat3(*conns_b[0]);

        ret = a - b;
    }
    else if (node_type == rttr::type::get<node::Script>())
    {
        auto text = static_cast<const node::Script&>(node).GetText();
        cpputil::StringHelper::ReplaceAll(text, "\\n", "\n");
        cpputil::StringHelper::ReplaceAll(text, "\\t", "\t");

        prepare_script_vars(node);
        try {
            ret = ScriptEnv::Instance()->GetChai()->eval<sm::vec3>(text);
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

void Evaluator::TopologicalSorting(std::vector<NodePtr>& nodes)
{
    // prepare
    std::vector<int> in_deg(nodes.size(), 0);
    std::vector<std::vector<int>> out_nodes(nodes.size());
    for (int i = 0, n = nodes.size(); i < n; ++i)
    {
        auto& node = nodes[i];
        for (auto& port : node->GetAllInput())
        {
            auto& conns = port->GetConnecting();
            if (conns.empty()) {
                continue;
            }

            assert(conns.size() == 1);
            auto& from = conns[0]->GetFrom()->GetParent();
            for (int j = 0, m = nodes.size(); j < m; ++j) {
                if (&from == nodes[j].get()) {
                    in_deg[i]++;
                    out_nodes[j].push_back(i);
                    break;
                }
            }
        }
    }

    // sort
    std::stack<int> st;
    std::vector<NodePtr> sorted;
    for (int i = 0, n = in_deg.size(); i < n; ++i) {
        if (in_deg[i] == 0) {
            st.push(i);
        }
    }
    while (!st.empty())
    {
        int v = st.top();
        st.pop();
        sorted.push_back(nodes[v]);
        for (auto& i : out_nodes[v]) {
            assert(in_deg[i] > 0);
            in_deg[i]--;
            if (in_deg[i] == 0) {
                st.push(i);
            }
        }
    }

    nodes = sorted;
}

}