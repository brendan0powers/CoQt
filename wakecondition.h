#ifndef WAKECONDITION_H
#define WAKECONDITION_H

#include <QObject>
#include <QSharedPointer>
#include <QDateTime>
#include <functional>

namespace CoQt
{
class Fiber;

class WakeCondition : public QObject
{
    Q_OBJECT
public:
    explicit WakeCondition(QSharedPointer<Fiber> parent);
    ~WakeCondition();

    virtual QDateTime wakeTime() = 0;
    virtual bool canWake() = 0;
    virtual void wake() = 0;

protected:
    QSharedPointer<Fiber> m_pFiber;
};

class ImmediateWakeConiditon : public WakeCondition
{
    Q_OBJECT
public:
    explicit ImmediateWakeConiditon(QSharedPointer<Fiber> parent);
    ~ImmediateWakeConiditon();

    virtual QDateTime wakeTime();
    virtual bool canWake();
    virtual void wake();

private:
    QDateTime m_wakeTime;
};

class SleepWakeConiditon : public WakeCondition
{
    Q_OBJECT
public:
    explicit SleepWakeConiditon(int iSleepMs, QSharedPointer<Fiber> parent);
    ~SleepWakeConiditon();

    virtual QDateTime wakeTime();
    virtual bool canWake();
    virtual void wake();

private:
    QDateTime m_wakeTime;
};

class LambdaWakeConiditon : public WakeCondition
{
    Q_OBJECT
public:
    explicit LambdaWakeConiditon(std::function<bool()> func, QSharedPointer<Fiber> parent, int iPollInterval = 0);
    ~LambdaWakeConiditon();

    virtual QDateTime wakeTime();
    virtual bool canWake();
    virtual void wake();

private:
    QDateTime m_wakeTime;
    std::function<bool()> m_func;
    int m_iPollInterval;
};

}

#endif // WAKECONDITION_H
