QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HTYPaint2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    graphicsscene.cpp \
    graphicsrectitem.cpp

HEADERS  += mainwindow.h \
    graphicsscene.h \
    graphicsrectitem.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

RC_FILE = res.rc