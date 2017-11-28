QT += core gui multimedia multimediawidgets svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = collections
TEMPLATE = app
CONFIG += c++11

DEFINES += QT_MESSAGELOGCONTEXT

unix {
    QT += dbus
    CONFIG += link_pkgconfig
    PKGCONFIG += dtkcore
}

SOURCES += main.cpp\
    mainwindow.cpp \
    buttontab.cpp \
    linetab.cpp \
    bartab.cpp \
    inputtab.cpp \
    slidertab.cpp \
    buttonlisttab.cpp \
    segmentedcontrol.cpp\
    indicatortab.cpp \
    widgetstab.cpp \
    comboboxtab.cpp \
    cameraform.cpp \
    simplelistviewtab.cpp \
    singlelistview.cpp \
    singlelistitem.cpp \
    multilistview.cpp \
    multilistitem.cpp \
    graphicseffecttab.cpp

HEADERS  += mainwindow.h \
    buttontab.h \
    linetab.h \
    bartab.h \
    inputtab.h \
    slidertab.h \
    buttonlisttab.h \
    segmentedcontrol.h \
    indicatortab.h \
    widgetstab.h \
    comboboxtab.h \
    cameraform.h \
    simplelistviewtab.h \
    singlelistview.h \
    singlelistitem.h \
    multilistview.h \
    multilistitem.h \
    graphicseffecttab.h

RESOURCES += \
    images.qrc \
    resources.qrc

FORMS += \
    cameraform.ui

include($$PWD/../../../src/config.pri)

win32* {
    CONFIG += no_lflags_merge
    DEFINES += STATIC_LIB

    #DEPENDS dtkcore
    INCLUDEPATH += $$INCLUDE_INSTALL_DIR\libdtk-$$VERSION\DCore
    LIBS += -L$$LIB_INSTALL_DIR -ldtkcore
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../src/release -ldtkwidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../src/debug -ldtkwidgetd
else:unix: LIBS += -L$$OUT_PWD/../../../src/ -ldtkwidget

INCLUDEPATH += $$PWD/../../../src
INCLUDEPATH += $$PWD/../../../src/widgets
INCLUDEPATH += $$PWD/../../../src/util
DEPENDPATH += $$PWD/../../../src

CONFIG(debug, debug|release) {
    unix:QMAKE_RPATHDIR += $$OUT_PWD/../../../src
}
