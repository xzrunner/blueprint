#pragma once

#include <ee0/AtomicOP.h>
#include <ee0/typedef.h>

namespace bp
{

class Pins;

class ConnectPinsAO : public ee0::AtomicOP
{
public:
    ConnectPinsAO(const ee0::SubjectMgrPtr& sub_mgr, 
        const std::shared_ptr<Pins>& from, const std::shared_ptr<Pins>& to);

    virtual void Undo() override;
    virtual void Redo() override;

private:
    ee0::SubjectMgrPtr m_sub_mgr;

    std::shared_ptr<Pins> m_from, m_to;

}; // ConnectPinsAO

}