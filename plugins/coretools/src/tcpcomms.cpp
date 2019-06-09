#include "tcpcomms.h"
#include "ui_tcpcommsform.h"

#include "network/ping.h"
#include "network/nicutils.h"
#include "network/ipvalidator.h"

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>

TcpComms::TcpComms(QObject *parent) :
    ui(new Ui::TcpCommsForm),
    m_tcpSocket(0),
    m_isOpen(false),
    m_userDisconnected(false)
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

    // Icons
    m_disconnectedIcon.addFile(":/connect_no.png", QSize(), QIcon::Disabled);
    m_disconnectedIcon.addFile(":/connect_creating.png", QSize(), QIcon::Normal);
    m_waitingIcon.addFile(":/working.png");
    m_connectedIcon.addFile(":/connect_established.png");

    // Configuration widget
    m_configWidget = new QWidget;
    ui->setupUi(m_configWidget);

    ui->btnConnect->setIcon(m_disconnectedIcon);

    // Set default values for ports
    m_userLocalPort = ui->spinLocalPort->value();
    m_userRemotePort = ui->spinRemotePort->value();

    // Timer for stream timeout
    m_timer.setSingleShot(true);
    m_timer.setInterval(100);
    connect(&m_timer, SIGNAL(timeout()),
            this, SLOT(timerExpired()));

    // Init TCP Server Listener
    m_tcpServer = new QTcpServer(this);
    m_tcpServer->setMaxPendingConnections(1);
    connect(m_tcpServer, SIGNAL(newConnection()),
            this, SLOT(inboundConnection()));

    // Connect UI slots
    // Local end of connection
    connect(ui->cmbNicList, SIGNAL(activated(int)),
            this, SLOT(localNicIndexChanged(int)));
    connect(ui->spinLocalPort, SIGNAL(editingFinished()),
            this, SLOT(localPortChanged()));

    // Remote end of connection
    QValidator *ipValid = new IpValidator(this);
    ui->edRemoteIp->setValidator(ipValid);
    connect(ui->edRemoteIp, SIGNAL(textEdited(QString)),
            this, SLOT(remoteIpEdited(QString)));
    connect(ui->edRemoteIp, SIGNAL(returnPressed()),
            this, SLOT(remoteIpReturnPressed()));
    connect(ui->spinRemotePort, SIGNAL(editingFinished()),
            this, SLOT(remotePortChanged()));

    // Buttons
    connect(ui->btnPing, SIGNAL(clicked()),
            this, SLOT(doPing()));
    connect(ui->btnConnect, SIGNAL(clicked()),
            this, SLOT(btnConnectClicked()));
}

TcpComms::~TcpComms()
{
    close();
    m_pingThread->quit();
    // Wait until it's stopped
    while(m_pingThread->isRunning());
}

QWidget * TcpComms::configWidget()
{
    return qobject_cast<QWidget *>(m_configWidget);
}

void TcpComms::open()
{
    m_isOpen = true;
    // Refresh NIC selection
    NicUtils::updateActiveNics(ui->cmbNicList);
    // Apply new default NIC selection
    localNicIndexChanged(ui->cmbNicList->currentIndex());
}

void TcpComms::close()
{
    if (m_isOpen)
    {
        emit note(QDateTime::currentDateTime(),
                  tr("TCP Server Stopped"),
                  QString(),
                  true);
    }
    m_isOpen = false;
    // Stop listening for connections
    stopServer();
    // Disconnect
    delete m_tcpSocket;
    m_tcpSocket = 0;
}

void TcpComms::transmit(const QByteArray &data)
{
    // Can only transmit if socket exists and is connected
    if (!m_tcpSocket)
    {
        emit note(QDateTime::currentDateTime(),
                  tr("Cannot send"),
                  tr("No Connection"),
                  false);
        return;
    }

    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState)
    {
        emit note(QDateTime::currentDateTime(),
                  tr("Cannot send"),
                  tr("TCP Socket not connected"),
                  false);
        return;
    }

    if (m_tcpSocket->write(data) == -1)
    {
        emit note(QDateTime::currentDateTime(),
                  tr("Send to %1 failed").arg(connectionLabel()),
                  m_tcpSocket->errorString(),
                  false);
        return;
    }
    // Success
    emit sentData(QDateTime::currentDateTime(),
                  QString(),
                  connectionLabel(),
                  data);
}

QString TcpComms::connectionLabel() const
{
    if (!m_tcpSocket)
        return tr("None");
    if (m_tcpSocket->state() == QAbstractSocket::UnconnectedState)
        return tr("Disconnected");
    return tr("%1:%2")
            .arg(m_tcpSocket->peerAddress().toString())
            .arg(m_tcpSocket->peerPort());
}

void TcpComms::disconnectLocalSocket()
{
    qDebug() << "Disconnecting local socket";
    if (m_tcpSocket)
        m_tcpSocket->disconnectFromHost();
}

void TcpComms::stopServer()
{
    // Stop listening to old one
    if (m_tcpServer->isListening())
    {
        qDebug() << "TCP stopping listening on" << m_tcpServer->serverAddress().toString()
                 << "port" << m_tcpServer->serverPort();
        m_tcpServer->close();
        emit note(QDateTime::currentDateTime(),
                  tr("TCP Server Stopped"),
                  QString(),
                  false);
    }
}

void TcpComms::setConnectMode()
{
    m_userDisconnected = false;
    if (!m_isOpen) return;
    if (ui->edRemoteIp->text().isEmpty())
    {   // We will wait for inbound connections
        setTcpListening();
        ui->spinRemotePort->setValue(0);
        ui->spinRemotePort->setEnabled(false);
        ui->spinLocalPort->setValue(m_userLocalPort);
        ui->spinLocalPort->setEnabled(true);
        ui->btnConnect->setEnabled(false);
    }
    else
    {   // We'll be doing outbound connections
        tryTcpRemote();
        stopServer();
        ui->spinRemotePort->setValue(m_userRemotePort);
        ui->spinRemotePort->setEnabled(true);
        ui->spinLocalPort->setValue(0);
        ui->spinLocalPort->setEnabled(false);
    }
}

void TcpComms::setTcpListening()
{
    if (m_tcpServer->isListening() &&
            m_tcpServer->serverAddress() == m_userLocalHostAddr &&
            m_tcpServer->serverPort() == m_userLocalPort) return;    // No change

    // Drop old connection if extant
    stopServer();
    disconnectLocalSocket();

    if (m_tcpServer->listen(m_userLocalHostAddr, m_userLocalPort))
    {
        qDebug() << "TCP Server Listening" << m_userLocalHostAddr.toString() << m_userLocalPort;
        emit note(QDateTime::currentDateTime(),
                  tr("TCP Server Ready"),
                  tr("Listening on IP %1:Port %2")
                  .arg(m_tcpServer->serverAddress().toString())
                  .arg(m_tcpServer->serverPort()),
                  true);
    }
    else
    {
        qDebug() << "TCP Server failed to start" << m_userLocalHostAddr.toString() << m_userLocalPort
                 << "Error:" << m_tcpServer->serverError() << m_tcpServer->errorString();
        emit note(QDateTime::currentDateTime(),
                  tr("TCP Failure"),
                  tr("Error: %1 %2").arg(m_tcpServer->serverError())
                  .arg(m_tcpServer->errorString()),
                  false);
    }
}

bool TcpComms::tryTcpRemote()
{
    if (m_userRemoteIp.protocol() == QAbstractSocket::IPv4Protocol)
    {   // Valid IP, send a Ping and enable Connect
        doPing();
        ui->btnConnect->setEnabled(true);
        return true;
    }
    // Invalid IP, disable Ping and Connect buttons
    ui->btnPing->setEnabled(false);
    ui->btnPing->setIcon(QIcon(":/working.png"));
    ui->btnPing->setToolTip(tr("Invalid IP"));
    ui->btnConnect->setEnabled(false);
    return false;
}

// Server and Socket Slots
// =======================

void TcpComms::setupSocketSignals()
{
    if (!m_tcpSocket) return;
    connect(m_tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(tcpSocketStateChanged(QAbstractSocket::SocketState)));
    connect(m_tcpSocket, SIGNAL(disconnected()),
            this, SLOT(tcpSocketDeleteDisconnected()));
    connect(m_tcpSocket, SIGNAL(readyRead()),
            this, SLOT(tcpDataReceived()));
}

void TcpComms::displaySocketSettings(QAbstractSocket *socket)
{
    if (socket)
    {   // Update the GUI to show the socket's connection settings
        ui->spinLocalPort->setValue(socket->localPort());
        // Find local IP
        ui->cmbNicList->setCurrentIndex(getNicListIndexOf(socket->localAddress()));
        ui->spinRemotePort->setValue(socket->peerPort());
        ui->edRemoteIp->setText(socket->peerAddress().toString());
    }
    else
    {   // Revert the GUI back to user-supplied settings
        ui->spinLocalPort->setValue(m_userLocalPort);
        ui->cmbNicList->setCurrentIndex(getNicListIndexOf(m_userLocalHostAddr));
        ui->spinRemotePort->setValue(m_userRemotePort);
        ui->edRemoteIp->setText(m_userRemoteIp.toString());
    }
}

int TcpComms::getNicListIndexOf(const QHostAddress &nicIp) const
{   // Find the index of the NIC
    QString nicIpStr(nicIp.toString());
    return ui->cmbNicList->findData(nicIpStr);
}

void TcpComms::inboundConnection()
{   // Server has received an inbound connection
    qDebug() << "Inbound Connection!";
    if (!m_tcpSocket)   // Ignore inbound connections if we already have one
    {   // Accept connection
        m_tcpSocket = m_tcpServer->nextPendingConnection();
        // Connect up the one we want
        setupSocketSignals();
        // Fire internal connected signal (the socket is already connected)
        if (m_tcpSocket->state() == QAbstractSocket::ConnectedState)
            tcpSocketStateChanged(QAbstractSocket::ConnectedState);
    }
    // Drop and delete any other pending ones
    while(m_tcpServer->hasPendingConnections())
    {
        QTcpSocket *tmpSock = m_tcpServer->nextPendingConnection();
        qDebug() << "Dropping unwanted inbound from" << tmpSock->peerAddress();
        delete tmpSock;
    }
}

void TcpComms::tcpSocketStateChanged(QAbstractSocket::SocketState newState)
{
    QString errString;
    int errCode = QAbstractSocket::UnknownSocketError;
    switch (newState)
    {
    case QAbstractSocket::UnconnectedState :
        ui->widEditors->setEnabled(true);
        ui->btnConnect->setIcon(m_disconnectedIcon);
        if (m_tcpSocket)
        {
            errCode = m_tcpSocket->error();
            errString = m_tcpSocket->errorString();
            m_tcpSocket = 0;
        }
        if ((errCode == QAbstractSocket::UnknownSocketError) &&
                m_userDisconnected)
            errString = tr("User Disconnected");
        emit note(QDateTime::currentDateTime(),
                  tr("TCP Disconnected"), errString, m_userDisconnected);
        ui->btnConnect->setText(tr("Connect"));
        displaySocketSettings();
        setConnectMode();
        break;
    case QAbstractSocket::HostLookupState : // Trying to connect
    case QAbstractSocket::ConnectingState :
        ui->widEditors->setEnabled(false);
        ui->btnConnect->setIcon(m_waitingIcon);
        ui->btnConnect->setText(tr("Connecting..."));
        ui->btnConnect->setEnabled(false);
        break;
    case QAbstractSocket::ConnectedState :
        ui->widEditors->setEnabled(false);
        ui->btnConnect->setIcon(m_connectedIcon);
        ui->btnConnect->setText(tr("Disconnect"));
        ui->btnConnect->setEnabled(true);  // So can disconnect
        displaySocketSettings(m_tcpSocket);
        if (m_tcpSocket)
        {
            // Always keep alive
            m_tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
            // Display local and
            emit note(QDateTime::currentDateTime(),
                      tr("TCP Connected"),
                      tr("Remote Host: %1").arg(connectionLabel()),
                      true);
        }
        break;
    case QAbstractSocket::BoundState :
        qDebug("TCP Socket is bound to an address and port (for servers)");
        break;
    case QAbstractSocket::ClosingState :
        qDebug("TCP Socket is about to close (data may still be waiting to be written)");
        break;
    default:
        qDebug() << "Socket changed to unknown state:" << newState;
    }
}

void TcpComms::tcpSocketDeleteDisconnected()
{
    // Erase the socket
    QObject::sender()->deleteLater();
}

void TcpComms::tcpDataReceived()
{
    if (!m_tcpSocket) return;   // This should never happen
    if (m_tcpSocket->bytesAvailable())
    {
        // Stop the clock
        m_timer.stop();

        m_rxBuffer.append(m_tcpSocket->readAll());
        // Split up
        char tmpChar = (char)ui->spinBoxSplitChar->value();
        QList<QByteArray> dataSplitList = m_rxBuffer.split(tmpChar);

        // Emit all the split bytes except the last
        for (int i=0; i< dataSplitList.count()-1; ++i)
        {
            // Split takes it away, so add dividing char back on
            QByteArray tmpData = dataSplitList.at(i);
            tmpData.append(tmpChar);

            emit receivedData(QDateTime::currentDateTime(),
                              connectionLabel(),
                              QString(),
                              tmpData);
        }
        m_rxBuffer = dataSplitList.last();
        // Ensure we never get *too* much data
        if (m_rxBuffer.size() > TCP_MAX_BUFFER_SIZE)
        {
            timerExpired();
        }

        // If still have data, start the clock
        if (m_rxBuffer.size() > 0)
        {
            m_timer.start();
        }
    }
}

void TcpComms::timerExpired()
{
    // Emit all the data we have right now
    emit receivedData(QDateTime::currentDateTime(),
                      connectionLabel(),
                      QString(),
                      m_rxBuffer);
    qDebug() << "TCP Comms Timer Expired";
    m_rxBuffer.clear();
}

// GUI Slots
// =========

void TcpComms::btnConnectClicked()
{   // Connect/Disconnect to remote server
    if (!m_tcpSocket)
    {   // Connecting
        qDebug() << "Manual Connection";
        // Create a new TCP Socket and try to connect it
        m_tcpSocket = new QTcpSocket(this);
        setupSocketSignals();
        m_tcpSocket->connectToHost(m_userRemoteIp, m_userRemotePort);
    }
    else
    {   // Disconnecting, so maybe start listening again?
        qDebug() << "Manual Disconnection";
        m_userDisconnected = true;
        m_tcpSocket->disconnectFromHost();
    }
}

void TcpComms::localNicIndexChanged(int index)
{
    m_userLocalHostAddr.setAddress(ui->cmbNicList->itemData(index).toString());
    if (!m_isOpen) return;
    if (m_tcpSocket) return;
    setConnectMode();
}

void TcpComms::localPortChanged()
{
    m_userLocalPort = ui->spinLocalPort->value();
    if (!m_isOpen) return;
    if (m_tcpSocket) return;
    setConnectMode();
}

void TcpComms::remoteIpEdited(const QString &text)
{
    m_userRemoteIp.setAddress(text);
    setConnectMode();
}

void TcpComms::remoteIpReturnPressed()
{
    if (!m_tcpSocket)
        btnConnectClicked();
}

void TcpComms::remotePortChanged()
{
    m_userRemotePort = ui->spinRemotePort->value();
    setConnectMode();
}

void TcpComms::doPing()
{
    ui->btnPing->setEnabled(false);
    ui->btnPing->setIcon(QIcon(":/working.png"));
    if (m_userRemoteIp.protocol() != QAbstractSocket::IPv4Protocol)
    {
        ui->btnPing->setToolTip(tr("Invalid IP"));
        return;
    }
    ui->btnPing->setToolTip(tr("Pinging..."));
    emit ping(m_userRemoteIp.toString(), 1000);
}

void TcpComms::onPingSuccess(QString host, int time)
{
    if (host != m_userRemoteIp.toString()) return;
    ui->btnPing->setEnabled(true);
    ui->btnPing->setIcon(QIcon(":/ok.png"));
    ui->btnPing->setToolTip(tr("%1 roundtrip %2ms")
                            .arg(host).arg(time));
}

void TcpComms::onPingFailure(QString host, QString message)
{
    if (host != m_userRemoteIp.toString()) return;
    ui->btnPing->setEnabled(true);
    ui->btnPing->setIcon(QIcon(":/warning.png"));
    ui->btnPing->setToolTip(tr("%1: %2").arg(host, message));
}
