QT += core gui openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    openglwidget.cpp \
    ../back/back.c

HEADERS += \
    mainwindow.h \
    openglwidget.h \
    ../back/back.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    3DviewerTK_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

win32:RC_FILE = /img/icon_win.rc
macx:ICON = icon_3D_macos.icns

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
