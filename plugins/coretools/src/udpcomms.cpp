#include "udpcomms.h"
#include "ui_udpcommsform.h"

#include "network/ping.h"
#include "network/nicutils.h"

#include <QThread>

UdpComms::UdpComms(QObject *parent) :
    ui(new Ui::UdpCommsForm),
    m_commMode(Mode_None),
    m_udpSocket(new QUdpSocket(this))
{
    Q_UNUSED(parent);
    // Create Pinger
    m_pinger = new Ping();
    m_pingThread = new QThread(this);
    connect(this, SIGNAL(ping(QString,int)),
            m_pinger, SLOT(ping(QString,int)));
    connect(m_pinger, SIGNAL(pingSuccess(QString,int)),
            this, SLOT(onPingSuccess(QString,int)));
    connect(m_pinger, SIGNAL(pingFailure(QString,QString)),
            this, SLOT(onPingFailure(QString,QString)));
    m_pinger->moveToThread(m_pingThread);
    m_pingThread->start();

    m_configWidget = new QWidget;
    ui->setupUi(m_configWidget);

    // Connect UI slots
    connect(m_udpSocket, SIGNAL(readyRead()),
            this, SLOT(udpDataReceived()));
    connect(m_udpSocket, SIGNAL(disconnected()),
            this, SLOT(disconnected()));

    connect(ui->cmbNicList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(nicChanged()));

    connect (ui->edTxIp, SIGNAL(textChanged(QString)),
             this, SLOT(edTxIpEdited(QString)));
    connect(ui->edTxIp, SIGNAL(editingFinished()),
            this, SLOT(destinationChanged()));

    connect(ui->spinTxPort, SIGNAL(editingFinished()),
            this, SLOT(destinationChanged()));

    connect(ui->spinRxPort, SIGNAL(editingFinished()),
            this, SLOT(rxPortChanged()));

    connect(ui->btnPing, SIGNAL(clicked()),
            this, SLOT(doPing()));

    edTxIpEdited(QString());

    // Populate NIC selection
    NicUtils::updateActiveNics(ui->cmbNicList);
}

UdpComms::~UdpComms()
{
    m_pingThread->quit();
    // Wait until it's stopped
    while(m_pingThread->isRunning());
}

QWidget * UdpComms::configWidget()
{
    return qobject_cast<QWidget *>(m_configWidget);
}

void UdpComms::open()
{
    // Bind to UDP
    rxPortChanged();
}

void UdpComms::close()
{
    // Disconnect
    m_udpSocket->disconnectFromHost();
}

void UdpComms::transmit(const QByteArray &data)
{
    QString dest = ui->edTxIp->text();
    quint16 port = ui->spinTxPort->value();
    if (m_udpSocket->writeDatagram(data,
                                   QHostAddress(dest),
                                   port) == -1)
    {
        emit note(QDateTime::currentDateTime(),
                  tr("Send to %1:%2 failed")
                  .arg(dest)
                  .arg(port), m_udpSocket->errorString(), false);
        return;
    }
    // Success
    emit sentData(QDateTime::currentDateTime(),
                  QString(),
                  QString("%1:%2").arg(dest).arg(port),
                  data);
}

void UdpComms::disconnected()
{
    emit note(QDateTime::currentDateTime(),
              tr("(UDP Disconnected)"), QString(), false);
}

void UdpComms::udpDataReceived()
{
    QByteArray tmpData;
    QHostAddress tmpHostAddr;
    quint16 tmpPort;
    while (m_udpSocket->hasPendingDatagrams())
    {
        tmpData.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(tmpData.data(), tmpData.size(), &tmpHostAddr, &tmpPort);
        // Emit
        emit receivedData(QDateTime::currentDateTime(),
                          QString("%1:%2").arg(tmpHostAddr.toString()).arg(tmpPort),
                          QString(),
                          tmpData);
    }
}

void UdpComms::onPingSuccess(QString host, int time)
{
    ui->btnPing->setEnabled(true);
    ui->btnPing->setIcon(QIcon(":/ok.png"));
    ui->btnPing->setToolTip(tr("%1 roundtrip %2ms")
                            .arg(host).arg(time));
}

void UdpComms::onPingFailure(QString host, QString message)
{
    ui->btnPing->setEnabled(true);
    ui->btnPing->setIcon(QIcon(":/warning.png"));
    ui->btnPing->setToolTip(tr("%1: %2").arg(host, message));
}

void UdpComms::nicChanged()
{

}

void UdpComms::rxPortChanged()
{
    qDebug("rxPortChanged");
    if (m_udpSocket->localPort() == ui->spinRxPort->value()) return;

    if (m_udpSocket->state() != QAbstractSocket::UnconnectedState)
    {
        qDebug("Rebinding with new port");
        m_udpSocket->disconnectFromHost();
    }
    if (m_udpSocket->bind(ui->spinRxPort->value(),
                QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
    {
        rebind(QHostAddress(ui->edTxIp->text()));
    }
}

void UdpComms::edTxIpEdited(const QString &arg1)
{
    QHostAddress dest(arg1);
    if (dest == prevAddress) return;
    ui->btnPing->setEnabled(false);
    ui->btnPing->setIcon(QIcon(":/working.png"));
    ui->btnPing->setToolTip("Invalid IP");
}

void UdpComms::doPing()
{
    ui->btnPing->setEnabled(false);
    ui->btnPing->setIcon(QIcon(":/working.png"));
    QHostAddress txAddress(ui->edTxIp->text());
    if (txAddress.protocol() != QAbstractSocket::IPv4Protocol)
    {
        ui->btnPing->setToolTip(tr("Invalid IP"));
        return;
    }
    ui->btnPing->setToolTip(tr("Pinging..."));
    emit ping(txAddress.toString(), 1000);
}

void UdpComms::destinationChanged()
{
    QHostAddress txAddress(ui->edTxIp->text());
    if (txAddress.protocol() != QAbstractSocket::IPv4Protocol)
    {
        prevAddress.clear();
        return;
    }
    if (txAddress != prevAddress)
    {
        if (txAddress.isInSubnet(QHostAddress("224.0.0.0"),4))
        {
            rebind(txAddress);
        }
        else
        {
            // Is a unicast, so ping it
            prevAddress = txAddress;
            doPing();
        }
    }
}

void UdpComms::rebind(QHostAddress newAddress)
{
    if (m_commMode == Mode_Multicast)
    {
        m_udpSocket->leaveMulticastGroup(prevAddress);
    }

    //Assume we fail
    m_commMode = Mode_None;

    if (newAddress.isInSubnet(QHostAddress("224.0.0.0"),4))
    {
        if (m_udpSocket->joinMulticastGroup(newAddress))
        {
            m_commMode = Mode_Multicast;
            emit note(QDateTime::currentDateTime(),
                      tr("UDP Multicast %1 Port %2")
                      .arg(ui->edTxIp->text())
                      .arg(m_udpSocket->localPort()), QString(), true);
        }
        else
        {
            prevAddress.clear();
            emit note(QDateTime::currentDateTime(),
                      tr("UDP Multicast Bind Fail"),
                      m_udpSocket->errorString(), false);
            return;
        }
    }
    else
    {
        m_commMode = Mode_Unicast;
        emit note(QDateTime::currentDateTime(),
                  tr("UDP Listening on Port %1")
                  .arg(m_udpSocket->localPort()), QString(), true);
    }
    prevAddress = newAddress;
}
