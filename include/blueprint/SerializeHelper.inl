#pragma once

namespace bp
{

template <typename T>
void SerializeHelper::SetupNodes(const std::vector<n0::SceneNodePtr>& nodes, std::vector<bp::NodePtr>& front_nodes,
                                 std::vector<std::shared_ptr<dag::Node<T>>>& back_nodes)
{
    for (int i = 0, n = nodes.size(); i < n; ++i)
    {
        if (!nodes[i]->HasUniqueComp<bp::CompNode>()) {
            continue;
        }

        auto front = nodes[i]->GetUniqueComp<bp::CompNode>().GetNode();
        front_nodes[i] = front;
        if (!front) {
            continue;
        }

        auto type = front->get_type();
        if (!type.is_derived_from<shaderlab::Node>()) {
            continue;
        }

        // create back node

        auto src_type = type.get_name().to_string();
        auto dst_type = "shadergraph::" + src_type.substr(src_type.find("shaderlab::") + std::string("shaderlab::").size());

        rttr::type t = rttr::type::get_by_name(dst_type);
        assert(t.is_valid());

        rttr::variant var = t.create();
        assert(var.is_valid());

        auto back = var.get_value<std::shared_ptr<dag::Node<T>>>();
        assert(back);

        // update back props
        auto f_type = front->get_type();
        auto b_type = back->get_type();
        if (f_type.is_derived_from<shaderlab::Node>() &&
            b_type.is_derived_from<dag::Node<T>>())
        {
            for (auto& dst_prop : b_type.get_properties())
            {
                auto src_prop = f_type.get_property(dst_prop.get_name());
                assert(src_prop.is_valid());
                dst_prop.set_value(back, src_prop.get_value(front));
            }
        }

        //shaderlab::ShaderAdapter::Front2Back(*front, *back, dir, dev);

        back_nodes[i] = back;
    }
}

template <typename T>
void SerializeHelper::SetupConnections(const std::string& filepath, const std::vector<n0::SceneNodePtr>& nodes,
                                       std::vector<bp::NodePtr>& front_nodes, std::vector<std::shared_ptr<dag::Node<T>>>& back_nodes)
{
    std::unordered_map<const bp::Node*, std::shared_ptr<dag::Node<shadergraph::Variant>>> front2back;
    assert(front_nodes.size() == back_nodes.size() && nodes.size() == front_nodes.size());
    for (int i = 0, n = nodes.size(); i < n; ++i)
    {
        if (front_nodes[i]) {
            assert(back_nodes[i]);
            front2back.insert({ front_nodes[i].get(), back_nodes[i] });
        }
    }

    rapidjson::Document doc;
    js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);
    bp::NSCompNode::LoadConnection(nodes, doc["nodes"]);
    for (int i = 0, n = nodes.size(); i < n; ++i)
    {
        auto& front = front_nodes[i];
        auto& back = back_nodes[i];
        if (!front) {
            continue;
        }

        assert(back);
        for (auto& in : front->GetAllInput())
        {
            for (auto& conn : in->GetConnecting())
            {
                auto f_pin = conn->GetFrom();
                auto t_pin = conn->GetTo();

                auto f_itr = front2back.find(&f_pin->GetParent());
                auto t_itr = front2back.find(&t_pin->GetParent());
                if (f_itr == front2back.end() || t_itr == front2back.end()) {
                    continue;
                }

                dag::make_connecting<shadergraph::Variant>(
                    { f_itr->second, f_pin->GetPosIdx() },
                    { t_itr->second, t_pin->GetPosIdx() }
                );
            }
        }
    }
}

}