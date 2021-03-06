TARGET = dtkwidget
TEMPLATE = lib

QT += dtkcore

# TODO: replace config.pri with dtk_build
include($$PWD/config.pri)
load(dtk_build)

CONFIG += internal_module

QT += multimedia multimediawidgets concurrent
greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets widgets-private
  # Qt >= 5.8
  greaterThan(QT_MAJOR_VERSION, 5)|greaterThan(QT_MINOR_VERSION, 7): QT += gui-private
  else: QT += platformsupport-private
}


linux* {
    QT += x11extras dbus
}

mac* {
    QT += svg dbus
    DEFINES += DTK_TITLE_DRAG_WINDOW
}

win* {
    QT += svg
    DEFINES += DTK_TITLE_DRAG_WINDOW
}

!isEmpty(DTK_STATIC_LIB){
    DEFINES += DTK_STATIC_LIB
    CONFIG += staticlib
}

load(configure)
qtCompileTest(libdframeworkdbus) {
    DEFINES += DFRAMEWORKDBUS_API_XEVENTMONITOR
} else {
    CONFIG -= no_keywords
}

HEADERS += dtkwidget_global.h

includes.path = $${DTK_INCLUDEPATH}/DWidget
includes.files += \
    $$PWD/dtkwidget_global.h\
    $$PWD/DtkWidgets\
    $$PWD/dtkwidget_config.h

include($$PWD/util/util.pri)
include($$PWD/widgets/widgets.pri)

linux* {
    includes.files += $$PWD/platforms/linux/*.h
}
win32* {
    includes.files += $$PWD/platforms/windows/*.h
}

# add translations
TRANSLATIONS += $$PWD/../translations/$${TARGET}2.ts \
                $$PWD/../translations/$${TARGET}2_zh_CN.ts

translations.path = $$PREFIX/share/$${TARGET}/translations
translations.files = $$PWD/../translations/*.qm

INSTALLS += translations

# create DtkWidgets file
defineTest(containIncludeFiles) {
    header = $$absolute_path($$ARGS)
    header_dir = $$quote($$dirname(header))

    for (file, includes.files) {
        file_ap = $$absolute_path($$file)
        file_dir = $$quote($$dirname(file_ap))

        isEqual(file_dir, $$header_dir):return(true)
    }

    return(false)
}

defineTest(updateDtkWidgetsFile) {
    dtkwidgets_include_files = $$HEADERS
    dtkwidgets_file_content = $$quote($${LITERAL_HASH}ifndef DTK_WIDGETS_MODULE_H)
    dtkwidgets_file_content += $$quote($${LITERAL_HASH}define DTK_WIDGETS_MODULE_H)

    for(header, dtkwidgets_include_files) {
        containIncludeFiles($$header) {
            dtkwidgets_file_content += $$quote($${LITERAL_HASH}include \"$$basename(header)\")
        }
    }

    dtkwidgets_file_content += $$quote($${LITERAL_HASH}endif)
    !write_file($$PWD/DtkWidgets, dtkwidgets_file_content):return(false)

    return(true)
}

!updateDtkWidgetsFile():warning(Cannot create "DtkWidgets" header file)

# create dtkwidget_config.h file
defineTest(updateDtkWidgetConfigFile) {
    for(file, includes.files) {
        file = $$quote($$basename(file))

        !isEqual(file, DtkWidgets):contains(file, D[A-Za-z0-9_]+) {
            dtkwidget_config_content += $$quote($${LITERAL_HASH}define DTKWIDGET_CLASS_$$file)
        }
    }

    !write_file($$PWD/dtkwidget_config.h, dtkwidget_config_content):return(false)

    return(true)
}

!updateDtkWidgetConfigFile():warning(Cannot create "dtkwidget_config.h" header file)

INSTALLS += includes target

load(dtk_cmake)

QMAKE_PKGCONFIG_REQUIRES += dtkcore
load(dtk_module)
