#include "escformat.h"
#include "ui_escformat.h"

#include "esctable.h"

EscFormat::EscFormat(QWidget *parent) :
    DataEntry(parent),
    ui(new Ui::EscFormat)
{
    ui->setupUi(this);
    ui->lblDescription->setText(dataEntryDescription());
    connect(ui->edSendText, SIGNAL(returnPressed()),
            this, SIGNAL(transmit()));
}

EscFormat::~EscFormat()
{
    delete ui;
}

QByteArray EscFormat::byteArray()
{
    if (!ui->edSendText->isModified()) return m_buffer;

    m_buffer = ui->edSendText->text().toLatin1();
    unEscapeChars(m_buffer);

    return m_buffer;
}

void EscFormat::setByteArray(const QByteArray data)
{
    m_buffer = data;
    ui->edSendText->setText(toString(data));
}

QString EscFormat::toString(const QByteArray data)
{
    QByteArray tmpData = data;
    escapeChars(tmpData);
    return QString::fromLatin1(tmpData);
}

void EscFormat::escapeChars(QByteArray &data)
{
    for (int i=0; i < escTableSize; i++)
    {
        data.replace(escTable[i].value, escTable[i].label);
    }
}

void EscFormat::unEscapeChars(QByteArray &data)
{
    for (int i=0; i < escTableSize; i++)
    {
        data.replace(escTable[i].label, &escTable[i].value);
    }
}
