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

#include "textformat.h"
#include "ui_textformat.h"

#include "asciitable.h"

TextFormat::TextFormat(QWidget *parent) :
    DataEntry(parent),
    ui(new Ui::TextFormat)
{
    ui->setupUi(this);
    ui->lblDescription->setText(dataEntryDescription());
    connect(ui->edSendText, SIGNAL(returnPressed()),
            this, SIGNAL(transmit()));
}

TextFormat::~TextFormat()
{
    delete ui;
}

QByteArray TextFormat::byteArray()
{
    QByteArray tmpData(ui->edSendText->text().toLatin1());
    // Append terminating character
    tmpData.append(ui->spinTermChar->value());
    return tmpData;
}

void TextFormat::setByteArray(const QByteArray data)
{
    QByteArray tmpData = data;
    // Put final char into the spinbox
    char finalChar = tmpData.at(tmpData.count()-1);
    ui->spinTermChar->setValue(finalChar);
    tmpData.chop(1);
    // Erase all unprintables
    for (int i=0; i < asciiTableSize; ++i)
        tmpData.replace(asciiTable[i].value, "");
    ui->edSendText->setText(QString::fromLatin1(tmpData));
}

QString TextFormat::toString(const QByteArray data)
{
    QByteArray tmpData = data;
    replaceUnprintableChars(tmpData);
    return QString::fromLatin1(tmpData);
}

void TextFormat::replaceUnprintableChars(QByteArray &data)
{
    for (int i=0; i < asciiTableSize; i++)
    {
        data.replace(asciiTable[i].value, asciiTable[i].label);
    }
}
