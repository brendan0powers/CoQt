#ifndef CONTEXT_H
#define CONTEXT_H

#include <QObject>
#include <QStack>
#include <QSharedPointer>

namespace CoQt
{
class Fiber;
class Scheduler;

class Context : public QObject
{
    Q_OBJECT
public:
    explicit Context(QObject *parent = 0);
    ~Context();

    QSharedPointer<Fiber> curFiber();
    void pushFiber(QSharedPointer<Fiber> pFiber);
    QSharedPointer<Fiber> popFiber();

    void registerFiber(QSharedPointer<Fiber> pFiber);
    void unregisterFiber(QSharedPointer<Fiber> pFiber);
    QList<QSharedPointer<Fiber> > listFibers();


    Scheduler *scheduler();

private:
    QStack<QSharedPointer<Fiber> > m_lstCurFibers;
    QList<QSharedPointer<Fiber> > m_lstFibers;
    Scheduler *m_pScheduler;
};

class FiberTracker
{
public:
    FiberTracker(QSharedPointer<Fiber> pFiber);
    ~FiberTracker();
};

Context *context();

}

#endif // CONTEXT_H
