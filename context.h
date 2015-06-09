#ifndef CONTEXT_H
#define CONTEXT_H

#include <QObject>
#include <QStack>
#include <QSharedPointer>

namespace CoQt
{
class Fiber;
class Scheduler;

//Context
//
//The context class keeps track of the created fibers,
//as well as the fibers that are currently running. There
//is one context class per thread, and you can use the
//CoQt::context() function to get the instance for the
//current thread. You can also get access to the scheduler
//through the Context::scheduler() function

class Context : public QObject
{
    Q_OBJECT

    friend class Fiber;
    friend class FiberTracker;
    friend CoQt::Context *context();
public:
    ~Context();

    //Returns the currently running fiber, or
    //NULL if no fiber is active
    QSharedPointer<Fiber> curFiber();

    //List the fibers for the current thread
    //Fibers that have finished will not be
    //listed here
    QList<QSharedPointer<Fiber> > listFibers();

    //Returns a pointer to the scheduler object
    Scheduler *scheduler();

private:
    explicit Context(QObject *parent = 0);

    //Accesed by the Fiber class to put items on the
    //current running fibers stack
    void pushFiber(QSharedPointer<Fiber> pFiber);
    QSharedPointer<Fiber> popFiber();

    //Used by the Fiber class to register a fiber when
    //is is created
    void registerFiber(QSharedPointer<Fiber> pFiber);
    void unregisterFiber(QSharedPointer<Fiber> pFiber);

    QStack<QSharedPointer<Fiber> > m_lstCurFibers;
    QList<QSharedPointer<Fiber> > m_lstFibers;
    Scheduler *m_pScheduler;
};

//Private class used to keep track of when a fiber is running
class FiberTracker
{
public:
    FiberTracker(QSharedPointer<Fiber> pFiber);
    ~FiberTracker();
};

Context *context();

}

#endif // CONTEXT_H
