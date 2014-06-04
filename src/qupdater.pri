QT += core gui network

isEmpty(QUPD_APP_NAME) {
    warning("QtUpdater: falling back to application name '$$TARGET'")
    DEFINES += QUPD_APP_NAME=\\\"$$TARGET\\\"
} else {
    message("QtUpdate: building for application name '$$QUPD_APP_NAME'")
    DEFINES += QUPD_APP_NAME=\\\"$$QUPD_APP_NAME\\\"
}

isEmpty(QUPD_APP_VERSION) {
    warning("QtUpdater: falling back to application version '$$VERSION'")
    DEFINES += QUPD_APP_VERSION=\\\"$$VERSION\\\"
} else {
    message("QtUpdater: building for application version '$$QUPD_APP_VERSION'")
    DEFINES += QUPD_APP_VERSION=\\\"$$QUPD_APP_VERSION\\\"
}

DEPENDPATH +=  "$$PWD"
INCLUDEPATH += "$$PWD"

SOURCES += \
        $$PWD/qupd_updater.cpp \
        $$PWD/qupd_versioncomparator.cpp \
        $$PWD/qupd_platform.cpp \
        $$PWD/qupd_ignoredversions.cpp \
        $$PWD/qupd_availableupdate.cpp \
        $$PWD/qupd_feedmanager.cpp \
        $$PWD/qupd_newversionmanager.cpp \
        $$PWD/qupd_changelogmanager.cpp \
        $$PWD/ui/qupd_downloadupdatedialog.cpp \
        $$PWD/ui/qupd_updaterdialoglight.cpp \
        $$PWD/ui/qupd_updaterdialogfull.cpp

HEADERS += \
        $$PWD/qupd_updater.h \
        $$PWD/qupd_versioncomparator.h \
        $$PWD/qupd_platform.h \
        $$PWD/qupd_ignoredversions.h \
        $$PWD/qupd_availableupdate.h \
        $$PWD/ui/qupd_downloadupdatedialog.h \
        $$PWD/ui/qupd_updaterdialoglight.h \
        $$PWD/ui/qupd_updaterdialogfull.h

FORMS += \
        $$PWD/ui/qupd_downloadupdatedialog.ui \
        $$PWD/ui/qupd_updaterdialoglight.ui \
        $$PWD/ui/qupd_updaterdialogfull.ui

TRANSLATIONS += \
        $$PWD/qtupdater_fr.ts

CODECFORTR = UTF-8
