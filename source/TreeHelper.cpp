#include "blueprint/TreeHelper.h"
#include "blueprint/Node.h"
#include "blueprint/Pin.h"
#include "blueprint/Connecting.h"

#include <queue>

namespace bp
{

void TreeHelper::GetSuccessorNodes(const Node& root, std::set<const Node*>& result)
{
    std::queue<const Node*> buf;
    buf.push(&root);
    while (!buf.empty())
    {
        auto n = buf.front(); buf.pop();
        result.insert(n);

        for (auto& c : n->GetAllOutput()) {
            for (auto& conn : c->GetConnecting()) {
                buf.push(&conn->GetTo()->GetParent());
            }
        }
    }
}

void TreeHelper::GetPrecursorNodes(const Node& root, std::set<const Node*>& result)
{
    std::queue<const Node*> buf;
    buf.push(&root);
    while (!buf.empty())
    {
        auto n = buf.front(); buf.pop();
        result.insert(n);

        for (auto& c : n->GetAllInput()) {
            for (auto& conn : c->GetConnecting()) {
                buf.push(&conn->GetFrom()->GetParent());
            }
        }
    }
}

}