#ifndef ESCTABLE_H
#define ESCTABLE_H

#include <QString>

struct char_QString
{
    char value;
    QString label;
};

static const int escTableSize = 7;

static const char_QString escTable[] = {
    {0x00, "\\0"},
    {0x07, "\\a"},
    {0x08, "\\b"},
    {0x09, "\\t"},
    {0x0a, "\\n"},
    {0x0b, "\\v"},
    {0x0d, "\\r"}
};

#endif // ESCTABLE_H
