#include "blueprint/node/Commentary.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/RenderSystem.h"

#include <node2/RenderSystem.h>
#include <painting2/RenderSystem.h>
#include <cpputil/StringHelper.h>

namespace bp
{
namespace node
{

Commentary::Commentary()
    : Node("")
{
    m_style.width  = 512;
    m_style.height = 512;

    m_style.panel_bg_col = BG_COLOR_COMMENTARY;
}

void Commentary::Draw(const n2::RenderParams& rp) const
{
    Node::Draw(rp);

    if (!m_comment_title.empty()) {
        DrawCommentTitle(rp);
    }
}

void Commentary::SetCommentText(const std::string& str) 
{
    if (str.empty()) {
        return;
    }

    m_comment_text = str;
    m_title = str;
}

void Commentary::AddChild(const NodePtr& node)
{
    RemoveExpiredChild();

    for (auto& itr = m_children.begin(); itr != m_children.end(); ++itr) {
        if (itr->lock() == node) {
            return;
        }
    }
    m_children.push_back(node);
}

bool Commentary::RemoveChild(const NodePtr& node)
{
    RemoveExpiredChild();

    bool ret = false;
    for (auto& itr = m_children.begin(); itr != m_children.end(); ++itr) {
        if (itr->lock() == node) {
            ret = true;
            m_children.erase(itr);
            break;
        }
    }
    return ret;
}

void Commentary::RemoveExpiredChild()
{
    for (auto& itr = m_children.begin(); itr != m_children.end(); ) {
        if (itr->expired()) {
            itr = m_children.erase(itr);
        } else {
            ++itr;
        }
    }
}

void Commentary::DrawCommentTitle(const n2::RenderParams& rp) const
{
    sm::Matrix2D mat;

    const float scale = std::max(1.0f, rp.GetCamScale());
    mat.Scale(scale, scale);

    const float x = GetPos().x;
    const float y = GetPos().y + m_style.height * 0.5f + NodeLayout::TITLE_HEIGHT * 0.5f * scale;
    mat.Translate(x, y);

    auto& tb = RenderSystem::Instance()->GetTitleTB();
    pt2::RenderSystem::DrawText(m_comment_title, tb, mat, RenderSystem::COL_TEXT, RenderSystem::COL_ZERO);
}

}
}