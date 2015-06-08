#include <QCoreApplication>
#include <QDebug>
#include <QSharedPointer>
#include "fiber.h"
#include "scheduler.h"


int main(int argc, char * argv[])
{
    QCoreApplication app(argc, argv);

    QSharedPointer<CoQt::Fiber> pFiber1 = CoQt::createFiber([&]() {

       CoQt::createFiber([&]() {
           int i = 0;
           while(true) {
               i++;
               qDebug() << "3: " << i;
               CoQt::Fiber::yield();
           }
       });

       int i = 0;
       while(true) {
           i++;
           qDebug() << i;
           CoQt::Fiber::yield();
       }
    });

    CoQt::createFiber([&]() {
       int i = 0;
       while(true) {
           i++;
           qDebug() << "2: " << i;
           pFiber1->wake();
           CoQt::Fiber::yield();
       }
    });

    CoQt::createFiber([&]() {
       int i = 0;
       while(true) {
           i++;
           qDebug() << "FOUR: " << i;
           CoQt::Fiber::yield(2000);
       }
    });

    CoQt::createFiber([&]() {
       int i = 0;
       while(true) {
           i++;
           qDebug() << "LAMBDA: " << i;
           int j = 0;
           CoQt::Fiber::yield([&]() {
               j++;

               return (j == 40);
           });
       }
    });

    qDebug() << "Hello World!";
    return app.exec();
}

