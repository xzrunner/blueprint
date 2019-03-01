#pragma once

#include <node0/typedef.h>
#include <SM_Rect.h>

#include <vector>

namespace ee0 { class WxStagePage; }

namespace bp
{

class CommentaryNodeHelper
{
public:
    static void InsertNodeToCommentary(ee0::WxStagePage& stage);

    static void InsertNodeToCommentary(ee0::WxStagePage& stage,
        const n0::SceneNodePtr& node);

    static void InsertNodeToCommentary(const n0::SceneNodePtr& node,
        const std::vector<n0::SceneNodePtr>& comm_nodes);

    static std::vector<n0::SceneNodePtr> 
        QueryCommNodeByRect(ee0::WxStagePage& stage, const sm::rect& r);

}; // CommentaryNodeHelper

}