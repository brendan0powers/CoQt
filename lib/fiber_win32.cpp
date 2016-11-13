
#include "fiber_p.h"
#include "context.h"
#include <QDebug>
#include <QThreadStorage>
#include <Windows.h>
#include <QStack>

namespace CoQt {
    class FiberPrivatePlatform {
    public:
        void *pFiber;
        QStack<void *> stkWakers;
    };
}

using namespace CoQt;

//Holds the main fiber for each thread we've attempted
//to run fibers on
static QThreadStorage<void *> gMainFibers;

//_stdcall only for 32bit CPUs
#ifdef Q_PROCESSOR_X86_32
#define STDCALL __stdcall
#else
#define STDCALL
#endif

//Fiber main function, get's the pointer to the FiberPrivate
//object, and calls the fibers actual fiber function
STDCALL void fiberInit(void *pData)
{
   FiberPrivate *pFiber = (FiberPrivate *)pData;

   //Actual fiber function
   pFiber->function();

   //set the state to finished, and unregister fiber
   pFiber->finishFiber();

   //Win32 fibers shouldn't ever exit, so ensure we return
   //to the previous fiber
   pFiber->pauseFiber();
}

void FiberPrivate::init()
{
    platform = new FiberPrivatePlatform();

    //If this thread has not been converted to a fiber
    //do so now
    if(!gMainFibers.hasLocalData())
        gMainFibers.setLocalData(ConvertThreadToFiber(NULL));

    //Create the fiber with the default stack size, pass in this
    //private object as the fiber data
    platform->pFiber = CreateFiber(uiDefaultStackSize, fiberInit, this);
}

void FiberPrivate::cleanup()
{
    //Delete the fiber
    DeleteFiber(platform->pFiber);
    delete platform;
    platform = NULL;
}

//Actually yeild the fiber
void FiberPrivate::pauseFiber()
{
    //Return execution to whatever fiber woke this one
    SwitchToFiber(context()->curFiber()->qxt_d().platform->stkWakers.pop());
}


void FiberPrivate::wake()
{
   //Store the current fiber so we konw what fiber to switch
   //back to on the next yield
   platform->stkWakers.push(GetCurrentFiber());

   //Switch execution to the new fiber
   SwitchToFiber(platform->pFiber);
}

