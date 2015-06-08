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

class Scheduler : public QObject
{
    Q_OBJECT
public:
    explicit Scheduler(Context *parent = 0);
    ~Scheduler();

    void scheculeFiber(WakeCondition *pCondition);
    void unscheculeFiber(WakeCondition *pCondition);

public slots:
    void runFibers();

private:
    void calculateNextTick();

    Context *m_pContext;
    QTimer *m_pTickTImer;
    QDateTime m_dtNextTick;
    bool m_bScheduleModified;

    QList<QPointer<WakeCondition> > m_lstWakeConditions;
};

}

#endif // SCHEDULER_H
