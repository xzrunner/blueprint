#include "blueprint/DisconnectConnAO.h"
#include "blueprint/Connecting.h"
#include "blueprint/Pins.h"

#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>

namespace bp
{

DisconnectConnAO::DisconnectConnAO(const ee0::SubjectMgrPtr& sub_mgr, 
                                   const std::shared_ptr<Pins>& from, 
                                   const std::shared_ptr<Pins>& to)
    : m_sub_mgr(sub_mgr)
    , m_from(from)
    , m_to(to)
{
}

void DisconnectConnAO::Undo()
{
    make_connecting(m_from, m_to);

    m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void DisconnectConnAO::Redo()
{
    std::shared_ptr<Connecting> conn = nullptr;
    for (auto& c : m_from->GetConnecting()) {
        if (c->GetTo() == m_to) {
            conn = c;
            break;
        }
    }
    assert(conn);

    disconnect(conn);

    m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

}