#ifndef CONGOFORMAT_H
#define CONGOFORMAT_H

#include "dataentry.h"

namespace Ui {
class CongoFormat;
}

class CongoFormat : public DataEntry
{
    Q_OBJECT
    
public:
    explicit CongoFormat(QWidget *parent = 0);
    ~CongoFormat();

    QString dataEntryName() {return tr("Congo Trigger Format");}
    QString dataEntryDescription() {return tr("Space-separated hexadecimal and text characters as used in Congo Triggers.\r\n"
                                              "Eg 'c o n g o 0d'");}
    float dataEntryVersion() {return 1.2f;}

    //! Convert an arbitrary byte array to a displayable string
    QString toString(const QByteArray data);

    //! The bytearray to send
    QByteArray byteArray();
    void setByteArray(const QByteArray data);
    
private:
    Ui::CongoFormat *ui;

    QByteArray m_buffer;

};

#endif // CONGOFORMAT_H
