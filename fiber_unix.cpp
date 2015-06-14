#include "fiber_p.h"
#include "context.h"

#define _XOPEN_SOURCE 600
#include "ucontext.h"
#include <QDebug>
#include <QThread>

#define STACK_SIZE 1024*1024 * 8

namespace CoQt {

class FiberPrivatePlatform {
public:
    ucontext_t parent;
    ucontext_t child;
};

}

using namespace CoQt;

static void fiberInit()
{
    int pointerLow, pointerHigh;
    FiberPrivate *pFiber = NULL;

    qDebug() << "Got Here...";

    //re-assemble pointer from 2 32bit ints if necessary
    if(sizeof(void *) == 32)
    {
        pFiber = (FiberPrivate *) pointerLow;
    }
    else
    {
        pFiber = (FiberPrivate *) (((quint64) pointerHigh) << 32 || pointerLow);
    }

    swapcontext(&pFiber->platform->child, &fiber->platform->parent);

    pFiber->function();

    //set the state to finished, and unregister fiber
    pFiber->finishFiber();
}

void FiberPrivate::init()
{
    platform = new FiberPrivatePlatform();

    getcontext(&platform->child);
    platform->child.uc_link = 0;
    platform->child.uc_stack.ss_sp = malloc(STACK_SIZE);
    platform->child.uc_stack.ss_size = STACK_SIZE;
    platform->child.uc_stack.ss_flags = 0;

    quint32 pointerLow, pointerHigh;
    if(sizeof(void *) == 32)
    {
        //pointerLow = (quint32)this;
        pointerHigh = 0;
    }
    else
    {
        //pull pointer into two 32 bit ints
        pointerLow = (quint32)(((quint64)this) & 0xFFFFFFFF);
        pointerHigh = (quint32)((((quint64)this) & 0xFFFFFFFF00000000) >> 32);
    }

    makecontext(&platform->child, (void (*)()) &fiberInit, 0); // 2, pointerLow, pointerHigh);
    swapcontext(&platform->parent, &platform->child);
}

void FiberPrivate::cleanup()
{
    platform->child.uc_stack.ss_sp;

    delete platform;
    platform = NULL;
}

//Actually yeild the fiber
void FiberPrivate::pauseFiber()
{
    if(context()->curFiber())
    {
        swapcontext(&context()->curFiber()->qxt_d().platform->child, &context()->curFiber()->qxt_d().platform->parent);
    }
}


void FiberPrivate::wake()
{
    swapcontext(&platform->parent, &platform->child);
}

