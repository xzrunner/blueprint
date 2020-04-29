#pragma once

#include "blueprint/typedef.h"

#include <node0/typedef.h>

#include <string>

namespace ur { class Device; }

namespace bp
{

class StageFuncNode
{
public:
    void SetParentNode(const n0::SceneNodePtr& parent_node) {
        m_parent_node = parent_node;
    }
    void EnableInsert(bool enable) { m_insert_enable = enable; }

    void SetFilepath(const std::string& filepath);

    void InsertSceneObj(const ur::Device& dev, const n0::SceneNodePtr& node);
    void DeleteSceneObj(const n0::SceneNodePtr& node);
    void ClearSceneObj();

private:
    void UpdateParentAABB(const NodePtr& node);

private:
    // bp::node::Function
    n0::SceneNodePtr m_parent_node = nullptr;

    bool m_insert_enable = true;

}; // StageFuncNode

}