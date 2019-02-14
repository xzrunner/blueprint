#include "blueprint/DisconnectConnAO.h"
#include "blueprint/Connecting.h"

#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>

namespace bp
{

DisconnectConnAO::DisconnectConnAO(const ee0::SubjectMgrPtr& sub_mgr, 
                                   const std::shared_ptr<Connecting>& conn)
    : m_sub_mgr(sub_mgr)
    , m_conn(conn)
{
}

void DisconnectConnAO::Undo()
{
    auto from = m_conn->GetFrom();
    auto to   = m_conn->GetTo();
    m_conn = make_connecting(from, to);

    m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void DisconnectConnAO::Redo()
{
    assert(m_conn);
    disconnect(m_conn);

    m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

}