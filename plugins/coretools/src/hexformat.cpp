#include "hexformat.h"
#include "ui_hexformat.h"

#include <QDebug>

HexFormat::HexFormat(QWidget *parent) :
    DataEntry(parent),
    ui(new Ui::HexFormat)
{
    ui->setupUi(this);
    ui->lblDescription->setText(dataEntryDescription());
    connect(ui->edSendText, SIGNAL(returnPressed()),
            this, SIGNAL(transmit()));
    connect(ui->edEscChar, SIGNAL(editingFinished()),
            this, SLOT(escapeCharChanged()));
    m_escChar = ui->edEscChar->text().at(0).toLatin1();
}

HexFormat::~HexFormat()
{
    delete ui;
}

QByteArray HexFormat::byteArray()
{
    if (!ui->edSendText->isModified()) return m_buffer;

    m_buffer = ui->edSendText->text().toLatin1();
    QByteArray escPair(2, m_escChar);
    QByteArray escItem(1, m_escChar);
    m_buffer.replace(escPair, escItem.toPercentEncoding(QByteArray(), QByteArray(), m_escChar));
    m_buffer = QByteArray::fromPercentEncoding(m_buffer, m_escChar);

    ui->edSendText->setModified(false);
    return m_buffer;
}

void HexFormat::setByteArray(const QByteArray data)
{
    m_buffer = data;
    ui->edSendText->setText(toString(data));
}

void HexFormat::escapeCharChanged()
{
    if (ui->edEscChar->text().isEmpty())
        ui->edEscChar->setText("$");

    char newEscChar = ui->edEscChar->text().at(0).toLatin1();
    if (newEscChar == m_escChar) return;

    // Get data using old escape char
    QByteArray tmpData = byteArray();
    // Update send text
    m_escChar = newEscChar;
    setByteArray(tmpData);
    // Update UI
    emit stringFormatChanged();
}

QString HexFormat::toString(const QByteArray data)
{
    QByteArray escPair(2, m_escChar);
    QByteArray escItem(1, m_escChar);

    QByteArray result = data.toPercentEncoding(QByteArray(), QByteArray(), m_escChar);
    result.replace(escItem.toPercentEncoding(QByteArray(), QByteArray(), m_escChar), escPair);
    return QString(result);
}
