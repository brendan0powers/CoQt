#include "fiber_p.h"
#include <boost/coroutine/all.hpp>
#include "context.h"
#include <QDebug>

namespace CoQt {
    class FiberPrivatePlatform {
    public:
        boost::coroutines::asymmetric_coroutine<void>::pull_type coroutine;
        boost::coroutines::asymmetric_coroutine<void>::push_type* yield;
    };
}

using namespace CoQt;

void FiberPrivate::init()
{
    platform = new FiberPrivatePlatform();
    platform->yield = NULL;
    platform->coroutine = boost::coroutines::asymmetric_coroutine<void>::pull_type(
                [&](boost::coroutines::asymmetric_coroutine<void>::push_type& sink)
    {
       platform->yield = &sink;
       sink();

       function();

       //set the state to finished, and unregister fiber
       finishFiber();
    });
}

void FiberPrivate::cleanup()
{
    delete platform;
    platform = NULL;
}

//Actually yeild the fiber
void FiberPrivate::pauseFiber()
{
    //yield control back to the context that woke us
    (*context()->curFiber()->qxt_d().platform->yield)();
}


void FiberPrivate::wake()
{
    //switch to the fiber
    platform->coroutine();
}

