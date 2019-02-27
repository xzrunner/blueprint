#pragma once

#include "blueprint/Node.h"

namespace bp
{
namespace node
{

class Commentary : public Node
{
public:
    Commentary()
        : Node("Commentary")
    {
        m_style.width  = 512;
        m_style.height = 512;

        m_style.panel_bg_col = pt0::Color(60, 60, 60, 60);
    }

    auto& GetCommentText() const { return m_comment_text; }
    void  SetCommentText(const std::string& str) {
        m_comment_text = str;
        m_title = str;
    }

    void  SetWidth(float width) { m_style.width = width; }
    float GetWidth() const { return m_style.width; }
    void  SetHeight(float height) { m_style.height = height; }
    float GetHeight() const { return m_style.height; }

private:
    std::string m_comment_text;

    RTTR_ENABLE(Node)

}; // Commentary

}
}