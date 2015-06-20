#ifndef WAKECONDITION_H
#define WAKECONDITION_H

#include <QObject>
#include <QSharedPointer>
#include <QDateTime>
#include <functional>
#include <QFuture>
#include <QFutureWatcher>
#include <QDebug>

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

public slots:
    //The fiber is woken when this function is called. If the WakeCondition
    //has been registered with the scheduler, it will be removed from the
    //scheduler queue.
    virtual void wake();

protected:
    QSharedPointer<Fiber> m_pFiber;
};

//Wakes the fiber on the next scheduler tick
class ImmediateWakeCondition : public WakeCondition
{
    Q_OBJECT
public:
    explicit ImmediateWakeCondition(QSharedPointer<Fiber> parent);
    ~ImmediateWakeCondition();

    virtual QDateTime wakeTime();
    virtual bool canWake();

private:
    QDateTime m_wakeTime;
};

//Wakes the fiber no sooner than the next scheduler tick, or when iSleepMs
//has elapsed.
class SleepWakeCondition : public WakeCondition
{
    Q_OBJECT
public:
    explicit SleepWakeCondition(int iSleepMs, QSharedPointer<Fiber> parent);
    ~SleepWakeCondition();

    virtual QDateTime wakeTime();
    virtual bool canWake();

private:
    QDateTime m_wakeTime;
};

//Wakes the fiber when the function object returns true. The function is run
//every iPollInterval MS with a minimum poll interval being one scheduler tick.
class LambdaWakeCondition : public WakeCondition
{
    Q_OBJECT
public:
    explicit LambdaWakeCondition(std::function<bool()> func, QSharedPointer<Fiber> parent, int iPollInterval = 0);
    ~LambdaWakeCondition();

    virtual QDateTime wakeTime();
    virtual bool canWake();

private:
    QDateTime m_wakeTime;
    std::function<bool()> m_func;
    int m_iPollInterval;
};

//Waits on the outcome of a QFuture the fiber is worken if the QFuture is
//finished or canceled. This allows integration with any of the QtConcurrent
//functions that return a QFuture
template <class T>
class QFutureWakeCondition : public WakeCondition
{
public:
    explicit QFutureWakeCondition(QFuture<T> *pFuture, QSharedPointer<Fiber> parent)
      : WakeCondition(parent)
      , m_pFuture(pFuture)
      , m_pWatcher(new QFutureWatcher<T>())
    {
        m_pWatcher->setFuture(*m_pFuture);

        QObject::connect(m_pWatcher, &QFutureWatcher<T>::finished, this, &QFutureWakeCondition<T>::wake);
        QObject::connect(m_pWatcher, &QFutureWatcher<T>::canceled, this, &QFutureWakeCondition<T>::wake);
    }

    ~QFutureWakeCondition()
    {
        delete m_pWatcher;
    }

    virtual QDateTime wakeTime() { return QDateTime(); }
    virtual bool canWake() { return false; }

private:
    QFuture<T> *m_pFuture;
    QFutureWatcher<T> *m_pWatcher;
};

//A helper class that does nothing on it's own
//connect to it's wake() slot to wake the fiber
class SignalWakeCondition : public WakeCondition
{
public:
    explicit SignalWakeCondition(QSharedPointer<Fiber> parent);
    ~SignalWakeCondition();

    virtual QDateTime wakeTime();
    virtual bool canWake();
};

}

#endif // WAKECONDITION_H
