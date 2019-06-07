#ifndef ESCFORMAT_H
#define ESCFORMAT_H

#include "dataentry.h"

namespace Ui {
class EscFormat;
}

class EscFormat : public DataEntry
{
    Q_OBJECT
    
public:
    explicit EscFormat(QWidget *parent = 0);
    ~EscFormat();

    QString dataEntryName() {return tr("Escaped Text");}
    QString dataEntryDescription() {return tr("Escaped text, eg \\n \\r");}
    float dataEntryVersion() {return 1.0f;}

    //! Convert an arbitrary byte array to a displayable string
    QString toString(const QByteArray data);

    //! The bytearray to send
    QByteArray byteArray();
    void setByteArray(const QByteArray data);
    
private:
    Ui::EscFormat *ui;

    QByteArray m_buffer;

    void escapeChars(QByteArray &data);
    void unEscapeChars(QByteArray &data);

};

#endif // ESCFORMAT_H
