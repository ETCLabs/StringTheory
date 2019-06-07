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

#ifndef ASCIITABLE_H
#define ASCIITABLE_H

#include <QString>

struct char_QString
{
    char value;
    QString label;
};

static const int asciiTableSize = 33;

static const char_QString asciiTable[] = {
    {0x00, "<NUL>"},
    {0x01, "<SOH>"},
    {0x02, "<STX>"},
    {0x03, "<ETX>"},
    {0x04, "<EOT>"},
    {0x05, "<ENQ>"},
    {0x06, "<ACK>"},
    {0x07, "<BEL>"},
    {0x08, "<BS>"},
    {0x09, "<TAB>"},
    {0x0a, "<LF>"},
    {0x0b, "<VT>"},
    {0x0c, "<FF>"},
    {0x0d, "<CR>"},
    {0x0e, "<SO>"},
    {0x0f, "<SI>"},
    {0x10, "<DLE>"},
    {0x11, "<DC1>"},
    {0x12, "<DC2>"},
    {0x13, "<DC3>"},
    {0x14, "<DC4>"},
    {0x15, "<NAK>"},
    {0x16, "<SYN>"},
    {0x17, "<ETB>"},
    {0x18, "<CAN>"},
    {0x19, "<EM>"},
    {0x1a, "<SUB>"},
    {0x1b, "<ESC>"},
    {0x1c, "<FS>"},
    {0x1d, "<GS>"},
    {0x1e, "<RS>"},
    {0x1f, "<US>"},
    {0x7f, "<DEL>"}
};

#endif // ASCIITABLE_H
