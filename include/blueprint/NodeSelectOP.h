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
        ECS_WORLD_PARAM ee0::WxStagePage& stage,
        uint32_t cam_cfg = ee2::CamControlOP::DEFAULT_FLAG);

    virtual bool OnMouseLeftDClick(int x, int y) override;

private:
    virtual void AfterInsertSelected(const n0::SceneNodePtr& node) const override;
    virtual void AfterDeleteSelected(const n0::SceneNodePtr& node) const override;
    virtual void AfterClearSelection() const override;

    void ChangeReferenceHighlight(bool is_set_var, const std::string& name, bool set_highlight) const;
    void ChangeValueHighlight(bool is_set_var, const std::string& name, bool set_highlight) const;
    void ClearVarHighlight() const;

    void SelectAllTree(const NodePtr& root, bool successor) const;

    static void SetNodeBGColor(const NodePtr& node, const pt0::Color& color);

}; // NodeSelectOP

}