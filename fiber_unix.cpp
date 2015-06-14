#include "fiber_p.h"
#include "context.h"

//Functions in ucontext a depricated on OSX
//enable them by defining _XOPEN_SOURCE
#ifdef __APPLE__
#define _XOPEN_SOURCE 1
#endif

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

    //Run the real fiber function
    pFiber->function();

    //set the state to finished, and unregister fiber
    pFiber->finishFiber();

    //return context to waker, do not return
    swapcontext(&pFiber->platform->child, &pFiber->platform->parent);
}

void FiberPrivate::init()
{
    platform = new FiberPrivatePlatform();

    //Get a context structure and allocate a new stack
    getcontext(&platform->child);
    platform->child.uc_link = 0;
    platform->child.uc_stack.ss_sp = malloc(STACK_SIZE);
    platform->child.uc_stack.ss_size = STACK_SIZE;
    platform->child.uc_stack.ss_flags = 0;

    //Create the new child context
    //We are passing a pointer to the makecontext function this is not
    //really supported by the POSIX APIs. On 64bit systems it will fail
    //However, this does work on GLibC 2.8 and above
    //A check should be added to cause a compile failure on unsupported
    //systems
    makecontext(&platform->child, (void (*)()) &fiberInit, 1, this);
}

void FiberPrivate::cleanup()
{
    //Free the stack
    free(platform->child.uc_stack.ss_sp);

    delete platform;
    platform = NULL;
}

//Actually yeild the fiber
void FiberPrivate::pauseFiber()
{
    //Switch back to the context that woke the fiber
    swapcontext(&context()->curFiber()->qxt_d().platform->child, &context()->curFiber()->qxt_d().platform->parent);
}


void FiberPrivate::wake()
{
    //switch from the current context to the fiber
    swapcontext(&platform->parent, &platform->child);
}

