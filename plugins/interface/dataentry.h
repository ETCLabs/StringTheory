/*
Copyright (c) 2010, Electronic Theatre Controls, Ltd.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of Electronic Theatre Controls, Ltd. nor the names of
   its contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QWidget>

class DataEntry : public QWidget
{
    Q_OBJECT
public:
    explicit DataEntry(QWidget *parent = 0):QWidget(parent) {}
    virtual ~DataEntry() {}

    //! Name of Data Entry method to show to user
    virtual QString dataEntryName() = 0;
    virtual QString dataEntryDescription() = 0;
    virtual float dataEntryVersion() = 0;

    //! Convert an arbitrary byte array to a displayable string
    virtual QString toString(const QByteArray data) = 0;

    //! The bytearray to send
    virtual QByteArray byteArray() = 0;
    virtual void setByteArray(const QByteArray data) = 0;

signals:
    //! Formatting used to convert QByteArry to QString has changed
    void stringFormatChanged();

    //! User wants to transmit
    void transmit();
};

#endif // DATAHANDLER_H
