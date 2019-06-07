# Copyright (c) 2018 Electronic Theatre Controls, Inc., http://www.etcconnect.com
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.


QT       += core gui network widgets

CONFIG += embed_manifest_exe

TARGET = StringTheory
TEMPLATE = app
DESTDIR = $$OUT_PWD/app

INCLUDEPATH += src plugins/interface

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/textformat.cpp \
    src/datatablemodel.cpp \
    src/aboutdialog.cpp \
    src/qtableviewcopy.cpp

HEADERS  += src/mainwindow.h \
    src/asciitable.h \
    src/textformat.h \
    plugins/interface/commsinterface.h \
    plugins/interface/genericcomms.h \
    plugins/interface/dataentryinterface.h \
    plugins/interface/dataentry.h \
    src/datatablemodel.h \
    src/version.h \
    src/aboutdialog.h \
    src/qtableviewcopy.h

FORMS    += ui/mainwindow.ui \
    ui/textformat.ui \
    ui/aboutdialog.ui

RC_FILE = res/resources.rc
OTHER_FILES += res/resources.rc

RESOURCES += \
    res/resources.qrc

# Extract the version using GIT tags
GIT_COMMAND = git --git-dir $$shell_quote($$PWD/.git) --work-tree $$shell_quote($$PWD)
GIT_VERSION = $$system($$GIT_COMMAND describe --always --tags)
GIT_DATE_DAY = $$system($$GIT_COMMAND show -s --date=format:\"%a\" --format=\"%cd\" $$GIT_VERSION)
GIT_DATE_DATE = $$system($$GIT_COMMAND show -s --date=format:\"%d\" --format=\"%cd\" $$GIT_VERSION)
GIT_DATE_MONTH = $$system($$GIT_COMMAND show -s --date=format:\"%b\" --format=\"%cd\" $$GIT_VERSION)
GIT_DATE_YEAR = $$system($$GIT_COMMAND show -s --date=format:\"%Y\" --format=\"%cd\" $$GIT_VERSION)
GIT_TAG = $$system($$GIT_COMMAND describe --abbrev=0 --always --tags)
GIT_SHA1 = $$system($$GIT_COMMAND rev-parse --short HEAD)

DEFINES += GIT_CURRENT_SHA1=\\\"$$GIT_VERSION\\\"
DEFINES += GIT_DATE_DAY=\\\"$$GIT_DATE_DAY\\\"
DEFINES += GIT_DATE_DATE=\\\"$$GIT_DATE_DATE\\\"
DEFINES += GIT_DATE_MONTH=\\\"$$GIT_DATE_MONTH\\\"
DEFINES += GIT_DATE_YEAR=\\\"$$GIT_DATE_YEAR\\\"


# Automatically handle installer generation

isEmpty(TARGET_EXT) {
    win32 {
        TARGET_CUSTOM_EXT = .exe
    }
    macx {
        TARGET_CUSTOM_EXT = .app
    }
} else {
    TARGET_CUSTOM_EXT = $${TARGET_EXT}
}

win32 {
    PRODUCT_VERSION = "$$GIT_VERSION"

    DEPLOY_DIR = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/deploy))
    DEPLOY_TARGET = $$shell_quote($$system_path($${OUT_PWD}/app/$${TARGET}$${TARGET_CUSTOM_EXT}))

    PLUGINS_SRC = $$shell_quote($$system_path($${OUT_PWD}/app/stPlugin_*.dll))
    PLUGINS_DST = $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/deploy/stPlugin_*.dll))

    PRE_DEPLOY_COMMAND += $${QMAKE_DEL_FILE} $${DEPLOY_DIR}\*.* /S /Q $$escape_expand(\\n\\t)
    PRE_DEPLOY_COMMAND += $$QMAKE_COPY $${DEPLOY_TARGET} $${DEPLOY_DIR} $$escape_expand(\\n\\t)
    PRE_DEPLOY_COMMAND += $$QMAKE_COPY $${PLUGINS_SRC} $${PLUGINS_DST} $$escape_expand(\\n\\t)

    DEPLOY_COMMAND = windeployqt
    DEPLOY_OPT = --dir $${DEPLOY_DIR} -serialport -network

    DEPLOY_INSTALLER = makensis /DPRODUCT_VERSION="$${PRODUCT_VERSION}" $$shell_quote($$system_path($${_PRO_FILE_PWD_}/install/win/install.nsi))
}
macx {
    VERSION = $$system(echo $$GIT_VERSION | sed 's/[a-zA-Z]//')
    TARGET_CUSTOM_EXT = .exe

    DEPLOY_DIR = $${_PRO_FILE_PWD_}/install/mac
    DEPLOY_TARGET = $${OUT_PWD}/$${TARGET}$${TARGET_CUSTOM_EXT}

    DEPLOY_COMMAND = macdeployqt
    DEPLOY_OPT = -serialport -network
    DEPLOY_CLEANUP = $${QMAKE_DEL_FILE} $${DEPLOY_DIR}/StringTheory*.dmg

    DEPLOY_INSTALLER = $${_PRO_FILE_PWD_}/install/mac/create-dmg --volname "StringTheory_Installer" --volicon "$${_PRO_FILE_PWD_}/res/icon.icns"
    DEPLOY_INSTALLER += --background "$${_PRO_FILE_PWD_}/res/mac_install_bg.png" --window-pos 200 120 --window-size 800 400 --icon-size 100 --icon $${TARGET}$${TARGET_CUSTOM_EXT} 200 190 --hide-extension $${TARGET}$${TARGET_CUSTOM_EXT} --app-drop-link 600 185
    DEPLOY_INSTALLER += $${DEPLOY_DIR}/StringTheory_$${VERSION}.dmg $${OUT_PWD}/$${TARGET}$${TARGET_CUSTOM_EXT}
}

CONFIG( release , debug | release) {
    QMAKE_POST_LINK += $${PRE_DEPLOY_COMMAND} $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $${DEPLOY_COMMAND} $${DEPLOY_TARGET} $${DEPLOY_OPT} $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $${DEPLOY_CLEANUP} $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $${DEPLOY_INSTALLER} $$escape_expand(\\n\\t)
}
