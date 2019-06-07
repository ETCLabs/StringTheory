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

#ifndef GENERICCOMMS_H
#define GENERICCOMMS_H

#include <QObject>
#include <QDateTime>

class GenericComms : public QObject
{
    Q_OBJECT
public:
    virtual ~GenericComms() {}

    virtual QString commType() = 0;
    virtual float commVersion() = 0;
    virtual QWidget * configWidget() = 0;
    
signals:
    void receivedData(const QDateTime timestamp,
                      const QString &source,
                      const QString &dest,
                      QByteArray data);

    void sentData(const QDateTime timestamp,
                  const QString &source,
                  const QString &dest,
                  QByteArray data);

    void note(const QDateTime timestamp,
              const QString &note,
              const QString &info,
              bool good);

public slots:
    virtual void open() = 0;
    virtual void close() = 0;

    virtual void transmit(QByteArray &data) = 0;
};

#endif // GENERICCOMMS_H
