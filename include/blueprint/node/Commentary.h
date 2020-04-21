#pragma once

#include "blueprint/Node.h"

namespace bp
{
namespace node
{

class Commentary : public Node
{
public:
    Commentary();

    virtual void Draw(const ur2::Device& dev, ur2::Context& ctx,
        const n2::RenderParams& rp) const override;

    auto& GetCommentTitle() const { return m_comment_title; }
    void  SetCommentTitle(const std::string& str) { m_comment_title = str; }

    auto& GetCommentText() const { return m_comment_text; }
    void  SetCommentText(const std::string& str);

    void  SetWidth(float width) { m_style.width = width; }
    float GetWidth() const { return m_style.width; }
    void  SetHeight(float height) { m_style.height = height; }
    float GetHeight() const { return m_style.height; }

    void AddChild(const NodePtr& node);
    bool RemoveChild(const NodePtr& node);
    auto& GetAllChildren() const { return m_children; }

private:
    void RemoveExpiredChild();

    void DrawCommentTitle(ur2::Context& ctx, const n2::RenderParams& rp) const;

private:
    std::string m_comment_title;
    std::string m_comment_text;

    std::vector<std::weak_ptr<Node>> m_children;

    RTTR_ENABLE(Node)

}; // Commentary

}
}