QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qUpdater
TEMPLATE = lib

DEFINES += QTUPDATER_LIBRARY

SOURCES += \
        qupd_updater.cpp \
        qupd_versioncomparator.cpp \
        qupd_platform.cpp \
        qupd_ignoredversions.cpp \
        qupd_availableupdate.cpp \
        qupd_feedmanager.cpp \
        qupd_newversionmanager.cpp \
        qupd_changelogmanager.cpp \
        ui/qupd_downloadupdatedialog.cpp \
        ui/qupd_updaterdialoglight.cpp \
        ui/qupd_updaterdialogfull.cpp

HEADERS += \
        qupd_updater.h \
        qupd_versioncomparator.h \
        qupd_platform.h \
        qupd_ignoredversions.h \
        qupd_availableupdate.h \
        ui/qupd_downloadupdatedialog.h \
        ui/qupd_updaterdialoglight.h \
        ui/qupd_updaterdialogfull.h

FORMS += \
        ui/qupd_downloadupdatedialog.ui \
        ui/qupd_updaterdialoglight.ui \
        ui/qupd_updaterdialogfull.ui

RESOURCES += \
        resources.qrc

TRANSLATIONS += \
        qupdater_fr.ts

CODECFORTR = UTF-8

#macx {
#CONFIG += lib_bundle

#FRAMEWORK_HEADERS.version = 0.1
#FRAMEWORK_HEADERS.files = $$HEADERS
#FRAMEWORK_HEADERS.path = ./src
#QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS
#}

#INSTALL += target
