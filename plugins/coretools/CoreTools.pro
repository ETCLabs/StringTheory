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

QT       += core gui network widgets serialport

TARGET = $$qtLibraryTarget(stPlugin_coretools)
TEMPLATE = lib
CONFIG  += plugin
DESTDIR = $$OUT_PWD/../../app

INCLUDEPATH += ../interface

win32:LIBS += iphlpapi.lib \
        ws2_32.lib

# Application

SOURCES += src/coretools.cpp \
    src/udpcomms.cpp \
    src/tcpcomms.cpp \
    network/ping.cpp \
    network/nicutils.cpp \
    src/serialcomms.cpp \
    src/hexformat.cpp \
    src/congoformat.cpp \
    src/escformat.cpp \
    src/rawhexformat.cpp \
    network/ipvalidator.cpp

HEADERS += src/coretools.h \
    ../interface/commsinterface.h \
    ../interface/genericcomms.h \
    network/ping.h \
    network/nicutils.h \
    src/udpcomms.h \
    src/tcpcomms.h \
    src/serialcomms.h \
    ../interface/dataentryinterface.h \
    ../interface/dataentry.h \
    src/hexformat.h \
    src/congoformat.h \
    src/escformat.h \
    src/esctable.h \
    src/rawhexformat.h \
    network/ipvalidator.h

FORMS += ui/udpcommsform.ui \
    ui/serialcommsform.ui \
    ui/tcpcommsform.ui \
    ui/hexformat.ui \
    ui/congoformat.ui \
    ui/escformat.ui \
    ui/rawhexformat.ui \

RESOURCES += \
    res/coretools.qrc
