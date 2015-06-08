#include "context.h"
#include <QThreadStorage>
#include "scheduler.h"

using namespace CoQt;

static QThreadStorage<Context *> g_strContexts;

Context::Context(QObject *parent)
    : QObject(parent)
    , m_pScheduler(new Scheduler(this))
{

}

Context::~Context()
{

}

QSharedPointer<Fiber> Context::curFiber()
{
    if(m_lstCurFibers.size() == 0)
        return QSharedPointer<Fiber>();

    return m_lstCurFibers.top();
}

void Context::pushFiber(QSharedPointer<Fiber> pFiber)
{
    m_lstCurFibers.push(pFiber);
}

QSharedPointer<Fiber> Context::popFiber()
{
    return m_lstCurFibers.pop();
}

void Context::registerFiber(QSharedPointer<Fiber> pFiber)
{
    m_lstFibers << pFiber;
}

void Context::unregisterFiber(QSharedPointer<Fiber> pFiber)
{
    if(m_lstFibers.contains(pFiber))
        m_lstFibers.removeAll(pFiber);
}

QList<QSharedPointer<Fiber> > Context::listFibers()
{
    return m_lstFibers;
}

Scheduler *Context::scheduler()
{
    return m_pScheduler;
}

FiberTracker::FiberTracker(QSharedPointer<Fiber> pFiber)
{
    CoQt::context()->pushFiber(pFiber);
}

FiberTracker::~FiberTracker()
{
    CoQt::context()->popFiber();
}

CoQt::Context *CoQt::context()
{
    if(!g_strContexts.hasLocalData())
        g_strContexts.setLocalData(new Context(NULL));

    return g_strContexts.localData();
}
