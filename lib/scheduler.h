#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QObject>
#include <QList>
#include <QDateTime>
#include <QPointer>

class QTimer;

namespace CoQt
{
class Fiber;
class Context;
class WakeCondition;

//Scheduler
//
//The schesuler class handles waking fibers
//that have yeielded. Some fibers may not use
//the scheduler object if they are woken manually
//or by a Qt signal.
//
//The Scheduler uses the WakeCondition::wakeTime()
//function to determine the next time the scheduler
//should run. If the next wake time is less than the
//minimum tick time from the last tick, the scheuler
//will wait untill the minimum tick interval has elapsed
//before trying to wake fibers again.

class Scheduler : public QObject
{
    Q_OBJECT

    friend class Fiber;
public:
    explicit Scheduler(Context *parent = 0);
    ~Scheduler();

    //Get and set the minimum tick time in MS
    int minimumTickTime();
    void setMinimumTickTime(int iTickTimeMs);

public slots:
    //Runs when a scheduler tick occurs. Will wake all of
    //the fibers who's WakeConditions allow it.
    void runFibers();

private:
    //Called by Fiber to scheduler a wakeup by the schedler
    void scheculeFiber(WakeCondition *pCondition);
    void unscheculeFiber(WakeCondition *pCondition);

    //Calculate the next scheduler tick based on the wake
    //conditions in the queue
    void calculateNextTick();

    Context *m_pContext;
    QTimer *m_pTickTImer;
    QDateTime m_dtNextTick;
    bool m_bScheduleModified;
    int m_iMinTickTime;

    QList<QPointer<WakeCondition> > m_lstWakeConditions;
};

}

#endif // SCHEDULER_H
