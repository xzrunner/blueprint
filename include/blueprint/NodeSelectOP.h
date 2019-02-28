#pragma once

#include "blueprint/typedef.h"

#include <ee2/NodeSelectOP.h>

namespace pt0 { class Color; }

namespace bp
{

class NodeSelectOP : public ee2::NodeSelectOP
{
public:
    NodeSelectOP(const std::shared_ptr<pt0::Camera>& camera,
        ECS_WORLD_PARAM ee0::WxStagePage& stage);

private:
    virtual void AfterInsertSelected(const n0::SceneNodePtr& node) const override;
    virtual void AfterDeleteSelected(const n0::SceneNodePtr& node) const override;
    virtual void AfterClearSelection() const override;

    void ChangeVarHighlight(bool is_set_var, const std::string& name, bool set_highlight) const;
    void ClearVarHighlight() const;

    static void SetNodeBGColor(const NodePtr& node, const pt0::Color& color);

}; // NodeSelectOP

}