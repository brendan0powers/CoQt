#ifndef WAKECONDITION_H
#define WAKECONDITION_H

#include <QObject>
#include <QSharedPointer>
#include <QDateTime>
#include <functional>

namespace CoQt
{
class Fiber;

//Wake Conditions
//
//Wake conditions allow a fiber to be woken based on an external event
//Some wake conditions will cause the fiber to be woken immediately. An
//example of this would be a fiber that wakes when a signal is emmited.
//Othe wake conditions use the Scheduler to wake the fiber after a certain
//period of time, or poll at an interval untill some condition is met.
//The most common wake conditions have helper yeild functions in the Fiber
//class


//Wake condition base class. Inherit this class to create a custom wake
//condition. The custom condition can then be passed to the yeld() function
//in the Fiber class.
class WakeCondition : public QObject
{
    Q_OBJECT
public:
    explicit WakeCondition(QSharedPointer<Fiber> parent);
    ~WakeCondition();

    //Returns the next time at which this wake condition shoudl be polled.
    //If the returned time is before the minimum tick interval of the scheduler
    //the WakeCondition will be polled on the next scheduler tick.
    virtual QDateTime wakeTime() = 0;

    //Run by the scheduler to determine if the fiber should be woken. If this
    //function returns true, wake() is called and the WakeCondition is removed
    //from the scheduler queue.
    virtual bool canWake() = 0;

    //The fiber is woken when this function is called. If the WakeCondition
    //has been registered with the scheduler, it will be removed from the
    //scheduler queue.
    virtual void wake();

protected:
    QSharedPointer<Fiber> m_pFiber;
};

//Wakes the fiber on the next scheduler tick
class ImmediateWakeConiditon : public WakeCondition
{
    Q_OBJECT
public:
    explicit ImmediateWakeConiditon(QSharedPointer<Fiber> parent);
    ~ImmediateWakeConiditon();

    virtual QDateTime wakeTime();
    virtual bool canWake();

private:
    QDateTime m_wakeTime;
};

//Wakes the fiber no sooner than the next scheduler tick, or when iSleepMs
//has elapsed.
class SleepWakeConiditon : public WakeCondition
{
    Q_OBJECT
public:
    explicit SleepWakeConiditon(int iSleepMs, QSharedPointer<Fiber> parent);
    ~SleepWakeConiditon();

    virtual QDateTime wakeTime();
    virtual bool canWake();

private:
    QDateTime m_wakeTime;
};

//Wakes the fiber when the function object returns true. The function is run
//every iPollInterval MS with a minimum poll interval being one scheduler tick.
class LambdaWakeConiditon : public WakeCondition
{
    Q_OBJECT
public:
    explicit LambdaWakeConiditon(std::function<bool()> func, QSharedPointer<Fiber> parent, int iPollInterval = 0);
    ~LambdaWakeConiditon();

    virtual QDateTime wakeTime();
    virtual bool canWake();

private:
    QDateTime m_wakeTime;
    std::function<bool()> m_func;
    int m_iPollInterval;
};

}

#endif // WAKECONDITION_H
