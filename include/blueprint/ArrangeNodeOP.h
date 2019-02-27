#pragma once

#include <ee2/ArrangeNodeOP.h>

namespace bp
{

class ArrangeNodeOP : public ee2::ArrangeNodeOP
{
public:
    ArrangeNodeOP(const std::shared_ptr<pt0::Camera>& camera,
        ee0::WxStagePage& stage, ECS_WORLD_PARAM const ee2::ArrangeNodeCfg& cfg,
        const std::shared_ptr<ee0::EditOP>& prev_op);

}; // ArrangeNodeOP

}