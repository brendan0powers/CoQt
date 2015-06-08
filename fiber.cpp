#include "fiber.h"
#include <QSharedPointer>
#include "scheduler.h"
#include "context.h"
#include "wakecondition.h"

using namespace CoQt;

Fiber::Fiber(std::function<void()> func, QObject *parent)
    : QObject(parent)
    , m_function(func)
    , m_yield(NULL)
    , m_state(FiberIdle)
    , m_pCurWaitCondition(NULL)
{
    m_coroutine = boost::coroutines::asymmetric_coroutine<void>::pull_type(
                [&](boost::coroutines::asymmetric_coroutine<void>::push_type& sink)
    {
       m_yield = &sink;
       sink();

       m_function();

       m_state = FiberFinished;
       emit finished();
       context()->unregisterFiber(m_wpThis.toStrongRef());
    });
}

Fiber::~Fiber()
{

}

Fiber::FiberState Fiber::getState()
{
    return m_state;
}

bool Fiber::isRunning()
{
    return (m_state == FiberRunning);
}

bool Fiber::isWaiting()
{
    return (m_state == FiberWaiting);
}

bool Fiber::isFinished()
{
    return (m_state == FiberFinished);
}

void Fiber::yield()
{
   yield(new ImmediateWakeConiditon(context()->curFiber()));
}

void Fiber::yield(int iMs)
{
   yield(new SleepWakeConiditon(iMs, context()->curFiber()));
}

void Fiber::yield(std::function<bool()> func, int iPollInterval)
{
    yield(new LambdaWakeConiditon(func, context()->curFiber(), iPollInterval));
}

void Fiber::yield(WakeCondition *pCondition)
{
    context()->scheduler()->scheculeFiber(pCondition);
    context()->curFiber()->m_pCurWaitCondition = pCondition;
    pauseFiber();
}

void Fiber::yieldForever()
{
    pauseFiber();
}

void Fiber::wake()
{
    FiberTracker tracker(m_wpThis.toStrongRef());

    if(m_pCurWaitCondition)
    {
        delete m_pCurWaitCondition;
        m_pCurWaitCondition = NULL;
    }

    m_state = FiberRunning;
    emit running();

    m_coroutine();

    m_state = FiberWaiting;
    emit waiting();
}

void Fiber::pauseFiber()
{
    if(context()->curFiber())
         (*context()->curFiber()->m_yield)();
}

QSharedPointer<Fiber> CoQt::createFiber(const std::function<void()> &func)
{
    QSharedPointer<Fiber> pFiber = QSharedPointer<Fiber>(new Fiber(func, NULL));
    pFiber->m_wpThis = pFiber.toWeakRef();
    context()->registerFiber(pFiber);

    pFiber->wake();

    return pFiber;
}

