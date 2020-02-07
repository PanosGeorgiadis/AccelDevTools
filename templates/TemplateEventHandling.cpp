//
// Copyright (c) 2009 by FEI Company
// All rights reserved. This file includes confidential and proprietary information of FEI Company.
//
// Author: Panagiotis Georgiadis
//

using boost::signals2::signal;
using boost::signals2::connection;
using boost::signals2::slot;
using boost::signals2::scoped_connection;

// \brief   Represents an object that performs actions which may trigger events.
//          It allows clients to subscribe a handler for the event
//          via the Connect...() function
class LowLevelObject
{
public:
    void DoAction();
    typedef signal<void ()> ActionPerformedSignal;
    connection ConnectActionPerformed(ActionPerformedSignal::slot_type func);
private:
    ActionPerformedSignal<void ()> m_sig;
};

void LowLevelObject::DoAction()
{
    std::cout << "Hello world!\n";
    m_sig();
}

void LowLevelObject::ConnectActionPerformed(ActionPerformedSignal::slot_type func)
{
    m_sig.connect(func)
}


struct Listener
{
    void Init();
    void MyHandler_onAction();
}

void Listener::Init()
{
    LowLevelObject obj;
    Listener listen;
    obj.Connect(listen.MyHandler_onAction);
    obj.DoAction();
}

void Listener::MyHandler_onAction()
{
    std::cout << "object fired an event\n";
}


struct ForwardingListener
{
    void Connect(eventHandler)
    {
        m_obj.Connect(eventHandler);
    }
private:
    LowLevelObject m_obj;
}
