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

#ifndef TEXTFORMAT_H
#define TEXTFORMAT_H

#include "dataentry.h"

namespace Ui {
class TextFormat;
}

class TextFormat : public DataEntry
{
    Q_OBJECT
    
public:
    explicit TextFormat(QWidget *parent = 0);
    ~TextFormat();

    QString dataEntryName() {return tr("Basic Text");}
    QString dataEntryDescription() {return tr("Simple text with a terminating character");}
    float dataEntryVersion() {return 1.0f;}

    //! Convert an arbitrary byte array to a displayable string
    QString toString(const QByteArray data);

    //! The bytearray to send
    QByteArray byteArray();
    void setByteArray(const QByteArray data);
    
private:
    Ui::TextFormat *ui;

    void replaceUnprintableChars(QByteArray &data);

};

#endif // TEXTFORMAT_H
