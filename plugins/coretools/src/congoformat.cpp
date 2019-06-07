#include "congoformat.h"
#include "ui_congoformat.h"

#include <QDebug>

CongoFormat::CongoFormat(QWidget *parent) :
    DataEntry(parent),
    ui(new Ui::CongoFormat)
{
//    m_bufferData = new QByteArray();
    ui->setupUi(this);
    ui->lblDescription->setText(dataEntryDescription());
    connect(ui->edSendText, SIGNAL(returnPressed()),
            this, SIGNAL(transmit()));
}

CongoFormat::~CongoFormat()
{
    delete ui;
//    delete m_bufferData;
}

QByteArray CongoFormat::byteArray()
{
    if (!ui->edSendText->isModified()) return m_buffer;

    QByteArray tmpData(ui->edSendText->text().toLatin1());
    // Split at every space

    QList<QByteArray> dataList = tmpData.split(' ');
    m_buffer.clear();
    QByteArray byte;
    foreach (byte, dataList)
    {
        if (byte.size() == 2)
        {
            m_buffer.append(QByteArray::fromHex(byte));
        }
        else if (byte.size() > 0)
        {
            m_buffer.append(byte.at(0));
        }
    }

    ui->edSendText->setModified(false);

    return m_buffer;
}

void CongoFormat::setByteArray(const QByteArray data)
{
    m_buffer = data;
    ui->edSendText->setText(toString(data));
}

QString CongoFormat::toString(const QByteArray data)
{
    QString result;

    // Replace all unprintables with the hex variant
    for(int i = 0; i < data.size(); ++i)
    {
        quint8 tmpChar = data.at(i);
        if ((tmpChar < 33) ||
                (tmpChar > 126))
        {
            result.append(QString("%1 ").arg(int(tmpChar), int(2), int(16), QChar('0')));
        }
        else
        {
            result.append(QString(QChar(tmpChar)) + " ");
        }
    }

    return result.trimmed();
}
