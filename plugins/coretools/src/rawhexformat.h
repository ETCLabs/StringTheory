#ifndef RAWHEXFORMAT_H
#define RAWHEXFORMAT_H

#include "dataentry.h"

namespace Ui {
class RawHexFormat;
}

class RawHexFormat : public DataEntry
{
    Q_OBJECT
    
public:
    explicit RawHexFormat(QWidget *parent = 0);
    ~RawHexFormat();

    QString dataEntryName() {return tr("Raw Hexadecimal");}
    QString dataEntryDescription() {return tr("Raw Hexadecimal format. Bytes (optionally) separated with a 'space'\r\n"
                                              "Eg: '45 54 43 20 69 73 20 67 72 65 61 74 21 0d' for 'ETC is great!'");}
    float dataEntryVersion() {return 1.0f;}

    //! Convert an arbitrary byte array to a displayable string
    QString toString(const QByteArray data);

    //! The bytearray to send
    QByteArray byteArray();
    void setByteArray(const QByteArray data);
    
private:
    Ui::RawHexFormat *ui;

    QByteArray m_buffer;

};

#endif // RAWHEXFORMAT_H
