#ifndef FIBER_P_H
#define FIBER_P_H

#include "fiber.h"

namespace CoQt {

class FiberPrivatePlatform;

class FiberPrivate : public QxtPrivate<Fiber>
{
public:
    QXT_DECLARE_PUBLIC(Fiber)

    FiberPrivate()
        : platform(NULL)
        , state(Fiber::FiberIdle)
        , pCurWaitCondition(NULL)
    {}

    void registerFiber();

    //Platform specific functions
    void init();
    void cleanup();
    static void pauseFiber();
    void wake();

    void finishFiber(); //Set fiber finished state

    FiberPrivatePlatform *platform;
    std::function<void()> function;
    Fiber::FiberState state;
    QWeakPointer<Fiber> wpThis;
    WakeCondition *pCurWaitCondition;

    static quint32 uiDefaultStackSize;
};

}

#endif // FIBER_P_H

