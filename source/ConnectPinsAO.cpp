#include "blueprint/ConnectPinsAO.h"
#include "blueprint/Connecting.h"

#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>

namespace bp
{

ConnectPinsAO::ConnectPinsAO(const ee0::SubjectMgrPtr& sub_mgr,
                             const std::shared_ptr<Pins>& from, 
                             const std::shared_ptr<Pins>& to,
                             const std::shared_ptr<Connecting>& conn)
    : m_sub_mgr(sub_mgr)
    , m_from(from)
    , m_to(to)
    , m_conn(conn)
{
}

void ConnectPinsAO::Undo()
{
    assert(m_conn);

    disconnect(m_conn);
    m_conn.reset();

    m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void ConnectPinsAO::Redo()
{
    m_conn = make_connecting(m_from, m_to);

    m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

}