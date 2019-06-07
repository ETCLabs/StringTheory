#include "rawhexformat.h"
#include "ui_rawhexformat.h"

#include <QDebug>

RawHexFormat::RawHexFormat(QWidget *parent) :
    DataEntry(parent),
    ui(new Ui::RawHexFormat)
{
    ui->setupUi(this);
    ui->lblDescription->setText(dataEntryDescription());
    connect(ui->edSendText, SIGNAL(returnPressed()),
            this, SIGNAL(transmit()));
}

RawHexFormat::~RawHexFormat()
{
    delete ui;
}

QByteArray RawHexFormat::byteArray()
{
    if (!ui->edSendText->isModified()) return m_buffer;

    m_buffer = QByteArray::fromHex(ui->edSendText->text().toLatin1());

    ui->edSendText->setModified(false);
    return m_buffer;
}

void RawHexFormat::setByteArray(const QByteArray data)
{
    m_buffer = data;
    ui->edSendText->setText(toString(data));
}

QString RawHexFormat::toString(const QByteArray data)
{
    QString result(data.toHex());
    for(int i=2; i<result.size(); i+=3)
    {
        result.insert(i,' ');
    }
    return result;
}
