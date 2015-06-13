#include "fiber_p.h"
#include "context.h"

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

static void fiberInit(void *pData)
{
    FiberPrivate *pFiber = (FiberPrivate *)pData;

    pFiber->function();

    //set the state to finished, and unregister fiber
    pFiber->finishFiber();

    //return context to waker, do not return
    swapcontext(&pFiber->platform->child, &pFiber->platform->parent);
}

void FiberPrivate::init()
{
    platform = new FiberPrivatePlatform();

    getcontext(&platform->child);
    platform->child.uc_link = 0;
    platform->child.uc_stack.ss_sp = malloc(STACK_SIZE);
    platform->child.uc_stack.ss_size = STACK_SIZE;
    platform->child.uc_stack.ss_flags = 0;

    makecontext(&platform->child, (void (*)()) &fiberInit, 1, this);
}

void FiberPrivate::cleanup()
{
    free(platform->child.uc_stack.ss_sp);

    delete platform;
    platform = NULL;
}

//Actually yeild the fiber
void FiberPrivate::pauseFiber()
{
    if(context()->curFiber())
        swapcontext(&context()->curFiber()->qxt_d().platform->child, &context()->curFiber()->qxt_d().platform->parent);
}


void FiberPrivate::wake()
{
    swapcontext(&platform->parent, &platform->child);
}

