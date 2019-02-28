#include "blueprint/node/Commentary.h"

namespace bp
{
namespace node
{

Commentary::Commentary()
    : Node("Commentary")
{
    m_style.width  = 512;
    m_style.height = 512;

    m_style.panel_bg_col = BG_COLOR_COMMENTARY;
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

}
}