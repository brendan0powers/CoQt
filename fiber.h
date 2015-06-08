#ifndef FIBER_H
#define FIBER_H

#include <QObject>
#include <functional>
#include <boost/coroutine/all.hpp>
#include <QSharedPointer>
#include <QWeakPointer>
#include "functional"

namespace CoQt
{
class WakeCondition;

class Fiber : public QObject
{
    Q_OBJECT
public:
    enum FiberState
    {
        FiberIdle,
        FiberRunning,
        FiberWaiting,
        FiberFinished
    };

    explicit Fiber(std::function<void()> func, QObject *parent = 0);
    ~Fiber();

    FiberState getState();
    bool isRunning();
    bool isWaiting();
    bool isFinished();

    static void yield();
    static void yield(int iMs);
    static void yield(std::function<bool()> func, int iPollInterval = 0);

    template <class T>
    static void yield(T *object,void (T::*pFunc)(), int iPollInterval = 0)
    {
        yeild(std::bind(pFunc, object), iPollInterval);
    }

    static void yield(WakeCondition *pCondition);

    static void yieldForever();


signals:
    void running();
    void waiting();
    void finished();
    void stateChanged(CoQt::Fiber::FiberState state);

public slots:
    void wake();

private:
    static void pauseFiber();

    std::function<void()> m_function;
    boost::coroutines::asymmetric_coroutine<void>::pull_type m_coroutine;
    boost::coroutines::asymmetric_coroutine<void>::push_type* m_yield;
    FiberState m_state;
    QWeakPointer<Fiber> m_wpThis;
    WakeCondition *m_pCurWaitCondition;

    friend QSharedPointer<Fiber> createFiber(const std::function<void ()> &);
};

QSharedPointer<Fiber> createFiber(const std::function<void()> &func);
template <class T>
static QSharedPointer<Fiber> createFiber(T *object,void (T::*pFunc)())
{
    return createFiber(std::bind(pFunc, object));
}

}

Q_DECLARE_METATYPE(CoQt::Fiber::FiberState);

#endif // FIBER_H
