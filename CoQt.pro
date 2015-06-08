TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += "/usr/local/Cellar/boost/1.56.0/include/"
LIBS += "-L/usr/local/Cellar/boost/1.56.0/lib/"
LIBS += -lboost_coroutine-mt -lboost_system-mt


SOURCES += main.cpp \
    fiber.cpp \
    scheduler.cpp \
    context.cpp \
    wakecondition.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    fiber.h \
    scheduler.h \
    context.h \
    wakecondition.h

