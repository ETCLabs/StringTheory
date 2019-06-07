#ifndef HEXFORMAT_H
#define HEXFORMAT_H

#include "dataentry.h"

namespace Ui {
class HexFormat;
}

class HexFormat : public DataEntry
{
    Q_OBJECT
    
public:
    explicit HexFormat(QWidget *parent = 0);
    ~HexFormat();

    QString dataEntryName() {return tr("Escaped Hex");}
    QString dataEntryDescription() {return tr("Text with escaped Hexadecimal. Eg '$0d' for a carriage return.\r\n"
                                              "Repeat escape character (eg '$$') for the actual escape symbol.");}
    float dataEntryVersion() {return 1.2f;}

    //! Convert an arbitrary byte array to a displayable string
    QString toString(const QByteArray data);

    //! The bytearray to send
    QByteArray byteArray();
    void setByteArray(const QByteArray data);

private slots:
    void escapeCharChanged();
    
private:
    Ui::HexFormat *ui;

    QByteArray m_buffer;
    char m_escChar;
};

#endif // HEXFORMAT_H
