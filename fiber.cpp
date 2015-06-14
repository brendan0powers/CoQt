#include "fiber.h"
#include <QSharedPointer>
#include "scheduler.h"
#include "context.h"
#include "wakecondition.h"
#include "fiber_p.h"

using namespace CoQt;

quint32 FiberPrivate::uiDefaultStackSize = 0;

Fiber::Fiber(std::function<void()> func, QObject *parent)
    : QObject(parent)
{
    QXT_INIT_PRIVATE(Fiber)

    qxt_d().function = func;
    qxt_d().init();
}

Fiber::~Fiber()
{
    qxt_d().cleanup();
}

Fiber::FiberState Fiber::getState()
{
    return qxt_d().state;
}

bool Fiber::isRunning()
{
    return (qxt_d().state == FiberRunning);
}

bool Fiber::isWaiting()
{
    return (qxt_d().state == FiberWaiting);
}

bool Fiber::isFinished()
{
    return (qxt_d().state == FiberFinished);
}

void Fiber::yield()
{
   if(!context()->curFiber())
       return;

   yield(new ImmediateWakeConiditon(context()->curFiber()));
}

void Fiber::yield(int iMs)
{
   if(!context()->curFiber())
      return;

   yield(new SleepWakeConiditon(iMs, context()->curFiber()));
}

void Fiber::yield(std::function<bool()> func, int iPollInterval)
{
    if(!context()->curFiber())
       return;

    yield(new LambdaWakeConiditon(func, context()->curFiber(), iPollInterval));
}

void Fiber::yield(QObject *obj, const char *signal)
{
    if(!context()->curFiber())
       return;

    SignalWakeCondition condition(context()->curFiber());
    connect(obj, signal, &condition, SLOT(wake()));

    yieldForever();
}

void Fiber::yield(WakeCondition *pCondition)
{
    if(!context()->curFiber())
       return;

    context()->scheduler()->scheculeFiber(pCondition);
    context()->curFiber()->qxt_d().pCurWaitCondition = pCondition;
    FiberPrivate::pauseFiber();
}

void Fiber::yieldForever()
{
    if(!context()->curFiber())
       return;

    FiberPrivate::pauseFiber();
}

void Fiber::wake()
{
    //Do not wake a finished fiber
    if(qxt_d().state == FiberFinished)
        return;

    FiberTracker tracker(qxt_d().wpThis.toStrongRef());

    if(qxt_d().pCurWaitCondition)
    {
        delete qxt_d().pCurWaitCondition;
        qxt_d().pCurWaitCondition = NULL;
    }

    qxt_d().state = FiberRunning;
    emit running();

    qxt_d().wake();

    qxt_d().state = FiberWaiting;
    emit waiting();
}

quint32 Fiber::getDefaultStackSize()
{
    return FiberPrivate::uiDefaultStackSize;
}

void Fiber::setDefaultStackSize(quint32 uiStackSize)
{
    FiberPrivate::uiDefaultStackSize = uiStackSize;
}

//Register a fiber with this threads context object
void FiberPrivate::registerFiber()
{
    context()->registerFiber(wpThis.toStrongRef());
}

void FiberPrivate::finishFiber()
{
    state = Fiber::FiberFinished;
    emit qxt_p().finished();
    context()->unregisterFiber(wpThis.toStrongRef());
}

QSharedPointer<Fiber> CoQt::createFiber(const std::function<void()> &func)
{
    QSharedPointer<Fiber> pFiber = QSharedPointer<Fiber>(new Fiber(func, NULL));
    pFiber->qxt_d().wpThis = pFiber.toWeakRef();
    pFiber->qxt_d().registerFiber();

    pFiber->wake();

    return pFiber;
}

